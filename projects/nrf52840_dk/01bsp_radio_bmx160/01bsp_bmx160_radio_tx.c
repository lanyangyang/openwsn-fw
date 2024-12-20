#include "board.h"
#include "radio.h"
#include "leds.h"
#include "sctimer.h"
#include "bmx160.h"
#include "i2c.h"
#include "string.h"

//=========================== defines =========================================
#define CHANNEL         3               ///< 11=2.405GHz
#define SAMPLE_PERIOD   (32768>>4)      // @32kHz = 1s
#define BUFFER_SIZE     0x08            // 2B*3 axis values + 2B ending with '\r\n'

enum {
    APP_FLAG_START_FRAME = 0x01,
    APP_FLAG_END_FRAME   = 0x02,
    APP_FLAG_TIMER       = 0x04,
};

typedef enum {
    APP_STATE_TX         = 0x01,
    APP_STATE_RX         = 0x02,
} app_state_t;

//=========================== variables =======================================

typedef struct {
    volatile uint8_t flags;
    uint8_t packet[BUFFER_SIZE];
    uint8_t packet_len;
    uint8_t dataToSend[BUFFER_SIZE];
    app_state_t state;
} app_vars_t;

app_vars_t app_vars;
uint8_t testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xAA, 0xBB};//debug
volatile PORT_TIMER_WIDTH next_compare_value;

//=========================== prototypes ======================================

void cb_startFrame(PORT_TIMER_WIDTH timestamp);
void cb_endFrame(PORT_TIMER_WIDTH timestamp);
void cb_timer(void);
volatile uint32_t timer_interrupt_count = 0; //debug
volatile uint32_t main_loop_count = 0;
//=========================== main ============================================

int mote_main(void) {
    int16_t tmp;
    uint8_t i;

    // initialize
    board_init();
    leds_init(); 

    // bmx160
    i2c_set_addr(BMX160_ADDR);
    bmx160_acc_config(0x0c);
    bmx160_gyr_config(0x08);
    bmx160_acc_range(0x8);
    bmx160_gyr_range(0x1);

    radio_setStartFrameCb(cb_startFrame);
    radio_setEndFrameCb(cb_endFrame);

    // start sctimer
    sctimer_set_callback(cb_timer);
    next_compare_value = sctimer_readCounter() + SAMPLE_PERIOD;
    sctimer_setCompare(next_compare_value);
    sctimer_enable();

    // initial radio
    radio_rfOn();
    radio_setFrequency(CHANNEL, FREQ_RX); // Start in RX mode
    radio_rxEnable();
    radio_rxNow();

    app_vars.state = APP_STATE_RX;

    while (1) {
        while (app_vars.flags == 0x00) {
            // board_sleep();
        }
        main_loop_count++;
        if (app_vars.flags & APP_FLAG_TIMER) {
            app_vars.flags &= ~APP_FLAG_TIMER;

            // Check if we are in RX mode
            if (app_vars.state == APP_STATE_RX) {
                // Stop listening
                radio_rfOff();

                // Read sensor data
                bmx160_read_9dof_data();
                i = 0;
                tmp = bmx160_read_gyr_x();
                app_vars.dataToSend[i++] = (uint8_t)((tmp >> 8) & 0x00FF);
                app_vars.dataToSend[i++] = (uint8_t)(tmp & 0x00FF);

                tmp = bmx160_read_gyr_y();
                app_vars.dataToSend[i++] = (uint8_t)((tmp >> 8) & 0x00FF);
                app_vars.dataToSend[i++] = (uint8_t)(tmp & 0x00FF);

                tmp = bmx160_read_gyr_z();
                app_vars.dataToSend[i++] = (uint8_t)((tmp >> 8) & 0x00FF);
                app_vars.dataToSend[i++] = (uint8_t)(tmp & 0x00FF);

                app_vars.dataToSend[i++] = '\r';
                app_vars.dataToSend[i++] = '\n';

                // Prepare packet
                app_vars.packet_len = sizeof(app_vars.packet);
                memcpy(app_vars.packet, app_vars.dataToSend, BUFFER_SIZE);
                //memcpy(app_vars.packet, testData, 8); // debug
    
                // Load packet and TX
                radio_loadPacket(app_vars.packet, app_vars.packet_len);
                radio_txEnable();
                radio_txNow();

                app_vars.state = APP_STATE_TX;
            }
        }

        if (app_vars.flags & APP_FLAG_START_FRAME) {
            app_vars.flags &= ~APP_FLAG_START_FRAME; 
            leds_sync_on(); 
        }

        if (app_vars.flags & APP_FLAG_END_FRAME) {
            app_vars.flags &= ~APP_FLAG_END_FRAME; 

            if (app_vars.state == APP_STATE_TX) {
                // Switch back to RX mode after transmission
                radio_rxEnable();
                radio_rxNow();
                app_vars.state = APP_STATE_RX;
            }

            leds_sync_off(); // end of a frame
        }
    }
}

//=========================== functions =======================================

void cb_startFrame(PORT_TIMER_WIDTH timestamp) {
    app_vars.flags |= APP_FLAG_START_FRAME; 
}

void cb_endFrame(PORT_TIMER_WIDTH timestamp) {
    app_vars.flags |= APP_FLAG_END_FRAME; 
}

void cb_timer(void) {
    app_vars.flags |= APP_FLAG_TIMER; 
    next_compare_value += SAMPLE_PERIOD;
    sctimer_setCompare(next_compare_value);
    timer_interrupt_count++;
}
