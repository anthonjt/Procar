#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "DC_Motor.h"

void PWM_initfun(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;  /*Enable the PORTC Clock*/
    SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;   /*Enable the FTM0 Clock*/
    PORTC->PCR[10] = PORT_PCR_MUX(3);     /*MUX = ALT 3*/
    BITBAND_REG(GPIOC->PDDR, 10) = 1;     //Make PTC pin 2 output
    FTM3->SC = 0;  						// Make sure its Off!
    FTM3->CNTIN = 0;  				    // starting value for counter
    FTM3->CNT= 0; 						// loads the counter with CNTIN
    FTM3->CONTROLS[6].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK; // FTM0_CH6
    FTM3->MOD = FTM3_MOD_VALUE; 		// set the mod value
    /* Optionally, if you want FTM to operate during debug, set the BDM mode */
    FTM3->CONF = FTM_CONF_BDMMODE(3); 	// Set FTM to operate normally in BDM mode
    FTM3->SC  |= FTM_SC_PS(FTM3_CLK_PRESCALE); 		// Divide the input clock down by 2^FTM0_CLK_PRESCALE
    FTM3->SC  |= FTM_SC_CLKS(FTM3_CH6_CLK_SOURCE);  // Use the system clk
    //FTM3->SC  |= FTM_SC_TOIE(1);
    FTM3->CONTROLS[6].CnV = FTM3_MOD_VALUE*0.15;//
}

void FTM3_IRQHandler (void){
   (void)FTM3->SC;
   FTM3->SC |= 0x0080;
}

void setFTM3CnV(float a){
	FTM3->CONTROLS[6].CnV = FTM3_MOD_VALUE*a;
}
