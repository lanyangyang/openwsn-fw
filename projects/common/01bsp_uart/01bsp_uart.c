/**
\brief This is a program which shows how to use the bsp modules for the board
       and UART.

\note: Since the bsp modules for different platforms have the same declaration,
       you can use this project with any platform.

Load this program on your board. Open a serial terminal client (e.g. PuTTY or
TeraTerm):
- You will read "Hello World!" printed over and over on your terminal client.
- when you enter a character on the client, the board echoes it back (i.e. you
  see the character on the terminal client) and the "ERROR" led blinks.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
*/

#include "stdint.h"
#include "stdio.h"
#include "string.h"
// bsp modules required
#include "board.h"
#include "uart.h"
#include "sctimer.h"
#include "leds.h"

//=========================== defines =========================================

#define SCTIMER_PERIOD     0xffff // 0xffff@32kHz = 2s
#define MAX_MESSAGE_LENGTH 64 // buff size for receive data
uint8_t re_message[MAX_MESSAGE_LENGTH]; // msg buffer
uint8_t buffer_ready = 0; // msg status
uint8_t idx = 0; // msg insert idx
//=========================== variables =======================================

typedef struct {
              uint8_t uart_lastTxByteIndex;
   volatile   uint8_t uartDone;
   volatile   uint8_t uartSendNow;
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
   
   // clear local variable
   memset(&app_vars,0,sizeof(app_vars_t));
    
   app_vars.uartSendNow = 1;
   
   // initialize the board
   board_init();
   
   // setup UART
   uart_setCallbacks(cb_uartTxDone,cb_uartRxCb);
   uart_enableInterrupts();
   
   // setup sctimer
   sctimer_set_callback(cb_compare);
   sctimer_setCompare(sctimer_readCounter()+SCTIMER_PERIOD);
   
   while(1) {
      
      // wait for timer to elapse
      while (app_vars.uartSendNow==0);
      app_vars.uartSendNow = 0;
      
      // send msg if enter is pressed
      if (buffer_ready == 1) {
          app_vars.uart_lastTxByteIndex = 0;
          app_vars.uartDone = 0;
          uart_writeByte(re_message[app_vars.uart_lastTxByteIndex]);
          buffer_ready = 0; 
          
      }

      // send string over UART
      app_vars.uartDone              = 0;
      app_vars.uart_lastTxByteIndex  = 0;
      
      while(app_vars.uartDone==0);
   }
}

//=========================== callbacks =======================================

void cb_compare(void) {
   
   // have main "task" send over UART
   app_vars.uartSendNow = 1;
   
   // schedule again
   sctimer_setCompare(sctimer_readCounter()+SCTIMER_PERIOD);
}

void cb_uartTxDone(void) {
   app_vars.uart_lastTxByteIndex++;
   if (app_vars.uart_lastTxByteIndex < idx)  {
      uart_writeByte(re_message[app_vars.uart_lastTxByteIndex]);
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

   // check the buffer size
   if (re_message < MAX_MESSAGE_LENGTH - 1) {
       re_message[idx++] = byte;  
       if (byte == '\n') {
           re_message[idx] = '\0';  
           buffer_ready = 1;  
           idx = 0;  
       }
       
   } else {
       
       idx = 0;
   }

   // echo that byte over serial
   uart_writeByte(byte);
   return 0;
}