/**
\brief This program shows the use of the "sctimer" bsp module.

Since the bsp modules for different platforms have the same declaration, you
can use this project with any platform.

Load this program onto your board, and start running. It will enable the sctimer. 
The sctimer is periodic, of period SCTIMER_PERIOD ticks. Each time it elapses:
    - the frame debugpin toggles
    - the error LED toggles

\author Tengfei Chang <tengfei.chang@eecs.berkeley.edu>, April 2017.
*/

#include "stdint.h"
#include "string.h"
#include "board.h"
#include "debugpins.h"
#include "leds.h"
#include "sctimer.h"

//=========================== defines =========================================

#define SCTIMER_PERIOD     0.5*32768 // @32kHz = 1s
#define LED1_INTERVAL      0.5*32768   
#define LED2_INTERVAL      32768     
#define LED3_INTERVAL      1.5*32768     
#define LED4_INTERVAL      2*32768    

uint32_t led1_counter = 0;
uint32_t led2_counter = 0;
uint32_t led3_counter = 0;
uint32_t led4_counter = 0;

//=========================== variables =======================================

typedef struct {
   uint16_t num_compare;
} app_vars_t;

app_vars_t app_vars;

//=========================== prototypes ======================================

void cb_compare(void);

//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int mote_main(void) {  
   
   // initialize board. 
   board_init();
   
   sctimer_set_callback(cb_compare);
   sctimer_setCompare(sctimer_readCounter()+SCTIMER_PERIOD);
   
   while (1) {
      board_sleep();
   }
}

//=========================== callbacks =======================================

void cb_compare(void) {
   
   led1_counter += SCTIMER_PERIOD;
   led2_counter += SCTIMER_PERIOD;
   led3_counter += SCTIMER_PERIOD;
   led4_counter += SCTIMER_PERIOD;

   // toggle pin
   debugpins_frame_toggle();
   
   // toggle led
   if (led1_counter >= LED1_INTERVAL) {
      leds_error_toggle();  
      led1_counter = 0;  // 重置计数器
   }
   
   if (led2_counter >= LED2_INTERVAL) {
      leds_sync_toggle();  
      led2_counter = 0;  // 重置计数器
   }
   if (led3_counter >= LED3_INTERVAL) {
      leds_radio_toggle();  
      led3_counter = 0;  // 重置计数器
   }
   if (led4_counter >= LED4_INTERVAL) {
      leds_debug_toggle();
      led4_counter = 0;  // 重置计数器
   }

   // increment counter
   app_vars.num_compare++;
  

   // schedule again
   sctimer_setCompare(sctimer_readCounter()+SCTIMER_PERIOD);
}
