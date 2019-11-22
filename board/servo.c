#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "servo.h"

void PWM_init(void)
{
	 	//SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;  /*Enable the PORTC Clock*/ enables in DC motor
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
	    //FTM0->SC  |= FTM_SC_TOIE(1); 					// Enable Timer Overflow interrupts
}

void FTM0_IRQHandler (void)
{
   //Clear interrupt flag
   (void)FTM0->SC;
   FTM0->SC |= 0x0080;                               /*FTM counter has overflow*/
}

void setFTM0CnV(float a){
    FTM0->CONTROLS[1].CnV = a; 	// Use 0.05, 0.075, 0.1
}
