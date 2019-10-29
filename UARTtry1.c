
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_uart.h"

void UART3_init(void);
void UART3_Transmit_Poll(uint8_t);
void UART3_string (char a[]);

int main(void){
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    UART3_init();

    while (1){
        PRINTF("\r\n Press any key to run\r\n");
        GETCHAR();
        UART3_Transmit_Poll('h');
        //UART3_Transmit_Poll('e');
        //UART3_Transmit_Poll('l');
        //UART3_Transmit_Poll('l');
        //UART3_Transmit_Poll('0');

        //getBlocks test
		uint8_t* getBlocksData = getBlocks();
        PRINTF("getBlocks test");
		for (int i=0; i<20; i++){
			PRINTF(" %02X", getBlocksData[i]);
		}
    }
}

void UART3_Transmit_Poll(uint8_t data) {
	while (!(UART3->S1 & UART_S1_TDRE_MASK)); // wait until transmit data register is empty
	UART3->D = data;
}

void UART3_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART3                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    (1);            //0x0400;    /* enable clock for UART0 */
    UART3->C2   = 0;                             //0x00       /* turn off UART0 while changing configurations */
    UART3->BDH  = 0x00;             //0x00;      /* UART_BDH_SBR()
    UART3->BDL  = 0x0C;             //0x0C;      /* SBR7:SBR0  = 0b00001100 - 315200 Baud */
    //UART3->C4   = UART_S1_OR(15);              //0x0F;      /* Over Sampling Ratio (15+1) */
    UART3->C1   = UART_C1_M(0);                 //0x00;      /* 8-bit data */
    UART3->C2   = UART_C2_TE(1)|UART_C2_RE(1); //|=0x0C;    /* enable transmit & Receive*/

    SIM->SCGC5     = SIM_SCGC5_PORTC(3);         //|= 0x0200; /* enable clock for PORTA */
    PORTC->PCR[17]  = PORT_PCR_MUX(3);            //0x0200;    /* make PTA2 UART0_Tx pin */
    PORTC->PCR[16]  = PORT_PCR_MUX(3);            //0x0200;    /* make PTA1 UART0_Rx pin */
}

void UART3_string (char* a){
	int len = sizeof(a)/sizeof(int);
	for(int i=0; i<len; i++){
		UART3_Transmit_Poll(a[i]);
	}
}
