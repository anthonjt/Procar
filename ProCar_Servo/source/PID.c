// This is a P controller, there is no derivative or Integral
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#define mod 2047 /* CHECK FOR CORRECTNESS */

int lastU = 4915;
int thisU;
float lastE = 0;
float thisE;
int a = 1; // This value will need to be experimentally determined

int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
    __disable_irq();        /* global disable IRQs */

//	SIM->SCGC5 |= 0x0800;       /* enable clock to Port C*/
//	SIM->SCGC6 |= 0x01000000;   /* enable clock to FTM0 */
//  PORTC->PCR[2] = 0x0400;     /* PTC2 used by FTM0 */

    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;	/*Enable the PORTD Clock*/
    SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;	/*Enable the FTM0 Clock*/
    PORTC->PCR[2] = PORT_PCR_MUX(4);		/*MUX = ALT 4*/

    NVIC_EnableIRQ(FTM0_IRQn);		/*Enable the FTM Interrupt*/
    FTM0->SC |= 0x004F;				/*Setting TOIE = 1,CLKS =  01, PS = 111*/
    FTM0->MOD = 32000;				/*Setting the Modulo register = 32000*/
    FTM0->CONTROLS[1].CnSC = 0x0028;				/*Setting MSB = 1, ELSnB = 1*/
    FTM0->CONTROLS[1].CnV = 500;					/*Value of the Channel*/

     int i;
     for (;;) {
         i++;						/*Just a Count*/
     }
     return 0;
 }

 float PID_Controller (void)
 {
	thisE = .5; 				// temporarily hard coded, should be calculated from pixy2
	thisU = lastU + a*thisE;    // actual PI calculation
	lastU = thisU; 				// set lastU to thisU for following calculation
	lastE = thisE; 				// set lastE to thisE for following calculation
	return thisU;				// this will be new MOD value
 }

 void FTM0_IRQHandler (void)
 {
   //Clear interrupt flag
 	unsigned long ChannelValue = FTM0->CONTROLS[1].CnV;			/*Take the value of the Channel to compare it*/
   (void)FTM0->SC;
   FTM0->SC |= 0x0080;								/*FTM counter has overflow*/
   if(ChannelValue < 32000)							/*Channel Value > Modulo Value*/
   {
 	  FTM0->CONTROLS[1].CnV += 500;								/*Add 500 to Channel*/
   }
   else
   {
 	  FTM0->CONTROLS[1].CnV = 0;									/*Set Channel in 0*/
   }
 }
