#include "UART3_d.h"
#include "board.h"

uint8_t lengthofsend;

void UART3_Transmit_Poll(uint8_t data) {
	while (!(UART3->S1 & UART_S1_TDRE_MASK)); // wait until transmit data register is empty
	UART3->D = data;
}
void UART3_Transmit_Poll_s(char data[]) {
	for(int i=0; (data[i] != '\0'); i++){
		lengthofsend = i+1;
	}
	UART3_Transmit_Poll('<');
	for(int i=0; i<lengthofsend; i++){
		UART3_Transmit_Poll(data[i]);
	}
	UART3_Transmit_Poll('>');
}

void UART3_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART3                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    (1);            //0x0400;    /* enable clock for UART0 */
    UART3->C2   = 0;
    UART3->BDH  = 0x00;             //0x00;
    UART3->BDL  = 0x0C;             //0x0C;      /* 315200 Baud */
    UART3->C1   = UART_C1_M(0);
    UART3->C2   = UART_C2_TE(1)|UART_C2_RE(1);

    SIM->SCGC5     = SIM_SCGC5_PORTC(3);
    PORTC->PCR[17]  = PORT_PCR_MUX(3);
    PORTC->PCR[16]  = PORT_PCR_MUX(3);
}

void UART3_string (char* a){
	int len = sizeof(a)/sizeof(int);
	for(int i=0; i<len; i++){
		UART3_Transmit_Poll(a[i]);
	}
}
