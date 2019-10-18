#include <board.h>
#include <peripherals.h>
#include <MK64F12.h>

void UART3_init(void);
void UART3_Transmit_Poll(uint8_t);

int main(void){
	UART3_init();
	UART3_Transmit_Poll('h');
	return 0;
}

void UART3_Transmit_Poll(uint8_t data) {
	while (!(UART0->S1 & UART_S1_TDRE_MASK)); // wait until transmit data register is empty
	UART3->D = data;
}

void UART3_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART3                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    (1);            //0x0400;    /* enable clock for UART0 */
    UART3->C2   = 0;                             //0x00       /* turn off UART0 while changing configurations */
    UART3->BDH  = 0x01;             //0x00;      /* UART_BDH_SBR()
    UART3->BDL  = 0x12;             //0x0C;      /* SBR7:SBR0  = 0b00001100 - 115200 Baud */
    //UART3->C4   = UART_S1_OR(15);              //0x0F;      /* Over Sampling Ratio (15+1) */
    UART3->C1   = UART_C1_M(0);                 //0x00;      /* 8-bit data */
    UART3->C2   = UART_C2_TE(1)|UART_C2_RE(1); //|=0x0C;    /* enable transmit & Receive*/

    SIM->SCGC5     = SIM_SCGC5_PORTC(3);         //|= 0x0200; /* enable clock for PORTA */
    PORTC->PCR[17]  = PORT_PCR_MUX(3);            //0x0200;    /* make PTA2 UART0_Tx pin */
    PORTC->PCR[16]  = PORT_PCR_MUX(3);            //0x0200;    /* make PTA1 UART0_Rx pin */
}
