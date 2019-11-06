#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#define mod 4094 //2047 /* CHECK FOR CORRECTNESS */
void PWM_init(void);
void ADC0_init(void);
volatile uint32_t result;
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

//    __disable_irq();        /* global disable IRQs */
//    SIM->SCGC5 |= 0x100;    /* enable clock to Port B */
//    PORTA->PCR[1] = 0x100; /* make PTA1 pin as GPIO */
//    PTA->PDDR |= 0x40000;   /* make PTA1 as output pin */
//    __enae_irq();         /* global enable IRQs */
    PWM_iblnit();             /* Configure PWM */
    ADC0_init();            /* Configure ADC0 */
    return 0 ;
}

void FTM0_IRQHandler(void) {
	NVIC_DisableIRQ(FTM0_IRQn);
	ADC0->SC1[0] = 0; /* start conversion on channel 0 */
	while (!(ADC0->SC1[0] & 0x80)) {} /* wait for conversion complete */
	result = ADC0->R[0]; /* read conversion result and clear COCO flag */
	/*----- DC Motor Control with PWM 0-100% duty cycle----------------*/
	FTM0->MOD = 60000;
	FTM0->CONTROLS[1].CnV = 1500+result*(3/2); /* Set up channel value 50% Duty cycle*/
	PTB->PTOR = 0x80000; /* toggle green LED */
	FTM0->SC |= 0x80; /* clear TOF */
	NVIC_EnableIRQ(FTM0_IRQn);
}

void PWM_init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;       /* enable clock to Port C*/
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;   /* enable clock to FTM0 */
	PORTD->PCR[5] = PORT_PCR_MUX(4);     /* PTC2 used by FTM0 */
    SIM->SOPT2 |= 0x01000000;   /* use MCGFLLCLK as timer counter clock */
    FTM0->SC = 0;               /* disable timer */
    FTM0->CONTROLS[1].CnSC = 0x20|0x08;   /* edge-aligned, pulse high MSB:MSA=10, ELSB:ELSA=10*/
    FTM0->MOD = mod;            /* Set up modulo register for 42.6 us period or 23.3 kHz rate */
    FTM0->CONTROLS[1].CnV = mod/2;  /* Set up default channel value for 50% duty cycle */
    FTM0->SC |= 0x80;           /* clear TOF */
    FTM0->SC |= 0x40;           /* enable timeout interrupt */
    FTM0->SC |= 0x0C;           /* enable timer with pre-scaler /16 for SERVO*/
    NVIC_EnableIRQ(FTM0_IRQn);  /* enable FTM0 interrupt in NVIC */
}
void ADC0_init(void)
{
	uint16_t calibration;
    SIM->SCGC5 |= 0x2000;       /* clock to PORTE */
    PORTE->PCR[20] = 0;         /* PTE20 analog input */
    SIM->SCGC6 |= 0x8000000;    /* clock to ADC0 */
    ADC0->SC2 &= ~0x40;         /* software trigger */
    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00; /* clock div by 4, long sample time, single ended 12 bit, bus clock */

    //Start Calibration
    ADC0->SC3 |= ADC_SC3_CAL_MASK;
	while (ADC0->SC3 & ADC_SC3_CAL_MASK) {
	// Wait for calibration to complete
	}
	// Finish off the calibration
	// Initialize a 16-bit variable in RAM
	calibration = 0x0;
	// Add the plus-side calibration results to the variable
	calibration += ADC0->CLP0;
	calibration += ADC0->CLP1;
	calibration += ADC0->CLP2;
	calibration += ADC0->CLP3;
	calibration += ADC0->CLP4;
	calibration += ADC0->CLPS;
	// Divide by two
	calibration /= 2;
	// Set the MSB of the variable
	calibration |= 0x8000;
	// Store the value in the plus-side gain calibration register
	ADC0->PG = calibration;
	// Repeat the procedure for the minus-side calibration value
	calibration = 0x0000;
	calibration += ADC0->CLM0;
	calibration += ADC0->CLM1;
	calibration += ADC0->CLM2;
	calibration += ADC0->CLM3;
	calibration += ADC0->CLM4;
	calibration += ADC0->CLMS;
	calibration /= 2;
	calibration |= 0x8000;
	ADC0->MG = calibration;
    //Done Calibration

	/* Reconfigure ADC0*/
    /* clock div by 4, long sample time, single ended 12 bit, bus clock */
    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
}

