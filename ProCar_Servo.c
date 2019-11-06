/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#define mod 32000 /* CHECK FOR CORRECTNESS */

void PWM_init(void);

int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("Hello World\n");

    __disable_irq();        /* global disable IRQs */
    PWM_init();             /* Configure PWM */

    int i;
    for (;;) {
        i++;						/*Just a Count*/
    }
    return 0 ;
}

void FTM0_IRQHandler(void) {
	//Clear interrupt flag
	unsigned long ChannelValue = FTM0->CONTROLS[1].CnV;	/*Take the value of the Channel to compare it*/
	(void)FTM0->SC;

	FTM0->SC |= 0x0080;									/*FTM counter has overflow*/
	if(ChannelValue < 32000)							/*Channel Value > Modulo Value*/
	{
		FTM0->CONTROLS[1].CnV += 500;					/*Add 500 to Channel*/
	}
	else
	{
		FTM0->CONTROLS[1].CnV = 0;						/*Set Channel in 0*/
	}
}

void PWM_init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; /* enable clock to Port D*/
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;  /* enable clock to FTM0 */
    PORTD->PCR[5] = PORT_PCR_MUX(4);    /* PTD5 used by FTM0 */

    NVIC_EnableIRQ(FTM0_IRQn);  		/* enable FTM0 interrupt in NVIC */
    FTM0->SC |= 0x004F;					/*Setting TOIE = 1,CLKS =  01, PS = 111*/
    FTM0->MOD = 32000;					/*Setting the Modulo register = 32000*/
    FTM0->CONTROLS[1].CnSC = 0x20|0x08;	/*Setting MSB = 1, ELSnB = 1*/
    FTM0->CONTROLS[1].CnV = 500;		/*Value of the Channel*/
}
