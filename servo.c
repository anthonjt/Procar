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
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "servo.h"

void PWM_init(void)
{
	SIM->SCGC5 |= 0x0800;       /* enable clock to Port C*/
	SIM->SCGC6 |= 0x01000000;   /* enable clock to FTM0 */
    PORTC->PCR[2] = 0x0400;     /* PTC2 used by FTM0 */
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
    SIM->SCGC5 |= 0x2000;       /* clock to PORTE */
    PORTE->PCR[20] = 0;         /* PTE20 analog input */
    SIM->SCGC6 |= 0x8000000;    /* clock to ADC0 */
    ADC0->SC2 &= ~0x40;         /* software trigger */
    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00; /* clock div by 4, long sample time, single ended 12 bit, bus clock */
}
