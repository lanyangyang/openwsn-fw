/**
\brief This is a program which shows how to use the bsp modules for the board
       and leds.

\note: Since the bsp modules for different platforms have the same declaration,
       you can use this project with any platform.

Load this program on your boards. The LEDs should start blinking furiously.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2014.
*/

#include "stdint.h"
#include "stdio.h"
// bsp modules required
#include "board.h"
#include "leds.h"

//--------DELAY----------//
void some_delay(uint16_t);

void some_delay(uint16_t aa) {
   volatile uint16_t delay;
   while(aa>0){
        for (delay=0xffff;delay>0;delay--);
        aa--;
   }
}

//-----------BLINK LED---------------//
int mote_main(void) {uint8_t i;
   
   board_init();

   leds_all_off();

   while(1){
        
        //LED1
        leds_error_on(); 
        some_delay(50);
        leds_error_off();
        some_delay(50);
        //LED2
        leds_sync_on(); 
        some_delay(50);
        leds_sync_off();
        some_delay(50);
        //LED3
        leds_radio_on(); 
        some_delay(50);
        leds_radio_off();
        some_delay(50);
        //LED4
        leds_debug_on(); 
        some_delay(50);
        leds_debug_off();
        some_delay(50);
   
   }
   
   // reset the board, so the program starts running again
   board_reset();
   
   return 0;
}
//--------------END---------------//
