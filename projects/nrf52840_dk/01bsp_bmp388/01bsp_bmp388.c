#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "board.h"
#include "leds.h"
#include "sctimer.h"
#include "i2c.h"
#include "uart.h"
#include "bmp388.h"

//=========================== defines =========================================

#define SAMPLE_PERIOD   (32768>>4)  // @32kHz = 1s
#define BUFFER_SIZE     0x08   //2B*3 axises value + 2B ending with '\r\n'

typedef struct {
    uint16_t num_compare;
    bool sampling_now;
    uint8_t data[8];  // To store temperature and pressure data
    float temperature;
    float pressure;

    uint8_t who_am_i;

    // UART specific
                 uint8_t uart_lastTxByteIndex;
    volatile     uint8_t uartDone;
    volatile     uint8_t uartSendNow;
    volatile     uint8_t uartToSend[BUFFER_SIZE];
} app_vars_t;

app_vars_t app_vars;

//=========================== prototypes ======================================

void cb_compare(void);
void cb_uartTxDone(void);
uint8_t cb_uartRxCb(void);

//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int mote_main(void) {

    // Initialize board
    board_init();

    i2c_set_addr(BMP388_I2C_ADDR);
    // Setup UART
    uart_setCallbacks(cb_uartTxDone, cb_uartRxCb);
    uart_enableInterrupts();

    // Initialize SCTimer
    sctimer_set_callback(cb_compare);
    sctimer_setCompare(sctimer_readCounter() + SAMPLE_PERIOD);

    // Always set address first
    //i2c_set_addr(BMP388_I2C_ADDR);

    // Should be 0x50 for BMP388
    app_vars.who_am_i = bmp388_who_am_i();
    if (app_vars.who_am_i != 0x50) {
        // Handle error: sensor not detected
        while (1) {
            leds_error_toggle();
        }
    }

    // Initialize BMP388 sensor
    bmp388_init();
    //bmp388_is_cmd_ready();
    double uncomp_temp=0, uncomp_press=0;
    //app_vars.preready = bmp388_is_pressure_data_ready();
    //app_vars.tmpready = bmp388_is_temperature_data_ready();
    while (1) {

        // Wait for timer to elapse
        while (app_vars.uartSendNow == 0);
        app_vars.uartSendNow = 0;

        // Read uncompensated temperature and pressure
        //bmp388_read_uncompensated_data(&uncomp_press, &uncomp_temp);
        bmp388_Read_Pressure_Temperature(&uncomp_press, &uncomp_temp);
        // Compensate the readings
        app_vars.temperature = uncomp_temp;
        app_vars.pressure = uncomp_press;

        // Prepare data to send over UART
        int16_t temp_int = (int16_t)(app_vars.temperature * 100);  // Convert to int16_t with 2 decimal places
        int32_t press_int = (int32_t)(app_vars.pressure);          // Pressure in Pa

        uint8_t i = 0;
        // Temperature (2 bytes)
        app_vars.uartToSend[i++] = (uint8_t)((temp_int >> 8) & 0x00FF);
        app_vars.uartToSend[i++] = (uint8_t)(temp_int & 0x00FF);

        // Pressure (4 bytes)
        app_vars.uartToSend[i++] = (uint8_t)((press_int >> 24) & 0x00FF);
        app_vars.uartToSend[i++] = (uint8_t)((press_int >> 16) & 0x00FF);
        app_vars.uartToSend[i++] = (uint8_t)((press_int >> 8) & 0x00FF);
        app_vars.uartToSend[i++] = (uint8_t)(press_int & 0x00FF);

        // Add newline characters
        app_vars.uartToSend[i++] = '\r';
        app_vars.uartToSend[i++] = '\n';

        // Send data over UART
        app_vars.uartDone = 0;
        app_vars.uart_lastTxByteIndex = 0;
        uart_writeByte(app_vars.uartToSend[app_vars.uart_lastTxByteIndex]);
        while (app_vars.uartDone == 0);
    }
}

//=========================== callbacks =======================================

void cb_compare(void) {
   
    // have main "task" send over UART
    app_vars.uartSendNow = 1;

    // schedule again
    sctimer_setCompare(sctimer_readCounter()+SAMPLE_PERIOD);
}

void cb_uartTxDone(void) {

    app_vars.uart_lastTxByteIndex++;
    if (app_vars.uart_lastTxByteIndex<sizeof(app_vars.uartToSend)) {
        uart_writeByte(app_vars.uartToSend[app_vars.uart_lastTxByteIndex]);
    } else {
        app_vars.uartDone = 1;
    }
}

uint8_t cb_uartRxCb(void) {
   
   uint8_t byte;
   
   // toggle LED
   leds_error_toggle();
   
   // read received byte
   byte = uart_readByte();
   
   // echo that byte over serial
   uart_writeByte(byte);
   
   return 0;
}