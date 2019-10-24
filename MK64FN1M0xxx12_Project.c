/**
 * @file    MK64FN1M0xxx12_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

void PWM_init(void);
void ADC0_init(void);
void DC_Motor(void);

int speed = 50;

#define mod 2047

int main(void) {
	__disable_irq();        // global disable IRQs /1
	UART0_init();
	PWM_init();                     // Configure PWM /
	__enable_irq();         // global enable IRQs /
	while(1) {

	}
}

void PWM_init(void) // Also enables the TOF interrupt
	{
	SIM->SCGC5 |= 0x0800;       // enable clock to Port C*/
	SIM->SCGC5 |= 0x2000;		// enable clock to Port E
	SIM->SCGC6 |= 0x07000000;   // enable clock to TPM0,1,2 */
	PORTC->PCR[2] = 0x0400;     // PTC2 used by TPM0 */
	PORTE->PCR[21] = 0x300;
	PORTE->PCR[23] = 0x300;
	SIM->SOPT2 |= 0x01000000;   // use MCGFLLCLK as timer counter clock */
	FTM3->SC = 0;               // disable timer */
	FTM3->CONTROLS[1].CnSC = 0x20|0x08;   /* edge-aligned, pulse high MSB:MSA=10, ELSB:ELSA=10*/
	FTM3->MOD = mod;            // Set up modulo register for 42.6 us period or 23.3 kHz rate */
	FTM3->CONTROLS[1].CnV = mod/10;  /* Set up default channel value for 50% dutycycle */
	FTM3->SC |= 0x80;           // clear TOF */
	FTM3->SC |= 0x0B;           // enable timer with prescaler /8 For DC motor and Buzzer*/
	NVIC->ISER[0] |= 0x00080000;
}

void DC_Motor(void){
	FTM3->MOD = mod/2;
	FTM3->CONTROLS[1].CnV = (mod*speed)/100;  // Set up channel value 50% Duty cycle
}

void ADC0_init(void)
	{
	SIM->SCGC5 |= 0x2000;       /* clock to PORTE */
	PORTE->PCR[20] = 0;         /* PTE20 analog input */
	PORTE->PCR[22] = 0;
	SIM->SCGC6 |= 0x8000000;    /* clock to ADC0 */
	ADC0->SC2 &= ~0x40;         /* software trigger */
	ADC0->SC3 = 0x8;			/* 4 sample average */
	ADC0->CFG1 = 0x20 	;		 /* clock div by 4, long sample time, single ended 12 bit, bus clock */
}
