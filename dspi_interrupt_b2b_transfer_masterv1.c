
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_uart.h"
#include "servo.h"
#include "DC_Motor.h"
#include <stdio.h>
#include "UART3_d.h"

int main(void){
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    UART3_init();

    while (1){
        PRINTF("\r\n Press any key to run\r\n");
        GETCHAR();

        //getBlocks test
		uint8_t* getBlocksData = getBlocks();
        PRINTF("getBlocks test");
		for (int i=0; i<20; i++){
			PRINTF(" %02X", getBlocksData[i]);
		}
		char data[100];
		snprintf(data, sizeof(data), "%02X%02X", getBlocksData[6], getBlocksData[7]);
		UART3_Transmit_Poll_s(data);
    }
}
