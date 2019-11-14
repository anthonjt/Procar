#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"


void PWM_init(void);
//void ADC0_init(void);

#define FTM0_MOD_VALUE 37500
#define FTM0_CLK_PRESCALE 0
#define FTM0_CH6_CLK_SOURCE 1

int main(void) {
	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
    __disable_irq();        /* global disable IRQs */
    PWM_init();

	while(1) {

	}
}

void PWM_init(void) // Also enables the TOF interrupt
	{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;       // enable clock to Port C*/
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;   // enable clock to FTM0 */
	PORTA->PCR[1] = PORT_PCR_MUX(3);     // PTA1 */
	FTM0->SC = 0;               // disable timer */
	FTM0->CNTIN = 0;  				    // starting value for counter
	FTM0->CNT= 0; 						// loads the counter with CNTIN
	FTM0->CONTROLS[6].CnSC = FTM_CnSC_MSB_MASK|FTM_CnSC_ELSB_MASK; //FTM0 channel 6
	FTM0->MOD = FTM0_MOD_VALUE;            // Set up modulo register for 42.6 us period or 23.3 kHz rate */
	FTM0->CONF = FTM_CONF_BDMMODE(3);
//	FTM0->SC |= 0x80;           // clear TOF */
//	FTM0->SC |= 0x40;           // enable timer with prescaler /8 For DC motor and Buzzer*/
//	FTM0->SC |= 0x0C;
	FTM0->SC  |= FTM_SC_PS(FTM0_CLK_PRESCALE); 		// Divide the input clock down by 2^FTM0_CLK_PRESCALE
    FTM0->SC  |= FTM_SC_CLKS(FTM0_CH6_CLK_SOURCE);  // Use the system clk
	FTM0->SC  |= FTM_SC_TOIE(1);
	NVIC_EnableIRQ(FTM0_IRQn);    					/*Enable the FTM Interrupt*/
	FTM0->CONTROLS[6].CnV = FTM0_MOD_VALUE/2;  /* Set up default channel value for 50% dutycycle */
}

void FTM0_IRQHandler(void){
	//NVIC->ISER[0] &= ~0x00080000;
//	FTM0->SC |= 0x0080;
//	int speed = 50;
//	FTM0->MOD = mod/2;
//	FTM0->CONTROLS[6].CnV = (mod*speed)/100;  // Set up channel value 50% Duty cycle
	(void)FTM0->SC;
	 FTM0->SC |= 0x0080;
}
