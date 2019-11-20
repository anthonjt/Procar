#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#define FTM0_MOD_VALUE 37500 // Value closest to 65535 without going over to reach 2ms (50Hz) delay (60MHz/2^PS X 50)
#define FTM0_CV_VALUE 18750 // 37500/18750 = 2
#define FTM0_CLK_PRESCALE 5 // Power used to reach MOD value (2^5). Lower = better resolution
#define FTM0_CH1_CLK_SOURCE 1
float lastU = 2812.5; // start servo at 90 degrees
float thisU;
float lastE = 0;
float thisE;
float Kp = 2.89351852; // This value will need to be experimentally determined
float Ki = 2.89351852; // This value will need to be experimentally determined
float Ts = 1;		   // change in time for integral

float PID_Controller (void)
{
	thisE = 300; 														// temporarily hard coded, should be calculated from pixy2
	thisU = lastU + (Kp + Ki*(2/Ts))*thisE + (-Kp+Ki*(2/Ts))*lastE;    // actual P calculation
	lastU = thisU; 				// set lastU to thisU for proceeding calculation
	lastE = thisE; 				// set lastE to thisE for proceeding calculation
	return thisU;				// this will be new MOD value
}

int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();

    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;  /*Enable the PORTC Clock*/
    SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;   /*Enable the FTM0 Clock*/
    PORTC->PCR[2] = PORT_PCR_MUX(4);     /*MUX = ALT 4*/
    BITBAND_REG(GPIOC->PDDR, 2) = 1;     //Make PTC pin 2 output

    FTM0->SC = 0;  						// Make sure its Off!
    FTM0->CNTIN = 0;  				    // starting value for counter
    FTM0->CNT= 0; 						// loads the counter with CNTIN
    FTM0->CONTROLS[1].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK; // FTM0_CH1
    FTM0->MOD = FTM0_MOD_VALUE; 		// set the mod value
    /* Optionally, if you want FTM to operate during debug, set the BDM mode */
    FTM0->CONF = FTM_CONF_BDMMODE(3); 	// Set FTM to operate normally in BDM mode
    FTM0->SC  |= FTM_SC_PS(FTM0_CLK_PRESCALE); 		// Divide the input clock down by 2^FTM0_CLK_PRESCALE
    FTM0->SC  |= FTM_SC_CLKS(FTM0_CH1_CLK_SOURCE);  // Use the system clk
    FTM0->SC  |= FTM_SC_TOIE(1); 					// Enable Timer Overflow interrupts
    NVIC_EnableIRQ(FTM0_IRQn);    					/*Enable the FTM Interrupt*/

     while (1){
    	    FTM0->CONTROLS[1].CnV = PID_Controller(); 	// Use 0.05, 0.075, 0.1
   }
     return 0;
}

void FTM0_IRQHandler (void)
{
   //Clear interrupt flag
   (void)FTM0->SC;
   FTM0->SC |= 0x0080;                               /*FTM counter has overflow*/
}
