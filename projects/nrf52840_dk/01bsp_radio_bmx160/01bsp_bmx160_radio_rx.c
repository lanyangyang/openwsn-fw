#include "board.h"
#include "radio.h"
#include "leds.h"
#include "sctimer.h"
#include "uart.h"
#include "string.h"

//=========================== defines =========================================

#define CHANNEL         3               
#define BUFFER_SIZE     8            

enum {
    APP_FLAG_START_FRAME = 0x01,
    APP_FLAG_END_FRAME   = 0x02,
};

typedef enum {
    APP_STATE_RX         = 0x01,
} app_state_t;

//=========================== variables =======================================

typedef struct {
    volatile uint8_t flags;
    uint8_t packet[BUFFER_SIZE];
    uint8_t packet_len;
    app_state_t state;
    volatile uint8_t uartDone;
    volatile uint8_t uartSendNow;
    uint8_t uartTxIndex;
} app_vars_t;

app_vars_t app_vars;

//=========================== prototypes ======================================

void cb_startFrame(PORT_TIMER_WIDTH timestamp);
void cb_endFrame(PORT_TIMER_WIDTH timestamp);
void cb_uart_tx_done(void);
uint8_t cb_uart_rx(void);
volatile uint32_t main_loop_count = 0; // debug
//=========================== main ============================================

int mote_main(void) {
    
    // initialize
    board_init();
    leds_init();
    memset((void*)&app_vars, 0, sizeof(app_vars_t));
    app_vars.uartDone = 1;

    // UART
    uart_setCallbacks(cb_uart_tx_done, cb_uart_rx);
    uart_enableInterrupts();

    radio_setStartFrameCb(cb_startFrame);
    radio_setEndFrameCb(cb_endFrame);

    // initial radio
    radio_rfOn();
    radio_setFrequency(CHANNEL, FREQ_RX);
    radio_rxEnable();
    radio_rxNow();

    app_vars.state = APP_STATE_RX;

    while (1) {
        while (app_vars.flags == 0x00) {
            // board_sleep();
        }
        
        if (app_vars.flags & APP_FLAG_END_FRAME) {
            app_vars.flags &= ~APP_FLAG_END_FRAME; 
            main_loop_count++;
            if (app_vars.state == APP_STATE_RX) {
                app_vars.packet_len = sizeof(app_vars.packet);
                int8_t rssi;
                uint8_t lqi;
                bool crc;

                radio_getReceivedFrame(
                    app_vars.packet,
                    &app_vars.packet_len,
                    sizeof(app_vars.packet),
                    &rssi,
                    &lqi,
                    &crc
                );
                uint8_t actualDataLen = app_vars.packet_len;

                // assign last two data 0x0D 0x0A
                app_vars.packet[actualDataLen - 2] = 0x0D;
                app_vars.packet[actualDataLen - 1] = 0x0A;

                // UART
                if (app_vars.uartDone == 1) {
                    app_vars.uartDone = 0;
                    app_vars.uartTxIndex = 0;
                    uart_writeByte(app_vars.packet[app_vars.uartTxIndex]);
                }

                radio_rxEnable();
                radio_rxNow();
                leds_sync_toggle(); // end of frame
            }
        }
    }
}

//=========================== callbacks =======================================

void cb_startFrame(PORT_TIMER_WIDTH timestamp) {
    app_vars.flags |= APP_FLAG_START_FRAME; 
}

void cb_endFrame(PORT_TIMER_WIDTH timestamp) {
    app_vars.flags |= APP_FLAG_END_FRAME; 
}

void cb_uart_tx_done(void) {
    app_vars.uartTxIndex++;
    if (app_vars.uartTxIndex < app_vars.packet_len) {
        uart_writeByte(app_vars.packet[app_vars.uartTxIndex]);
    } else {
        app_vars.uartDone = 1;
    }
}

uint8_t cb_uart_rx(void) {
    uint8_t byte;

    // toggle LED
    leds_error_toggle();

    // read received byte
    byte = uart_readByte();

    // echo that byte over serial
    uart_writeByte(byte);

    return 0;
}
