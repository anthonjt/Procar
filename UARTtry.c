#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"
#include "fsl_device_registers.h"
#include "MK64F12.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_uart.h"
//#include "mbed-drivers/mbed.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_DSPI_MASTER_BASEADDR SPI0
#define DSPI_MASTER_CLK_SRC DSPI0_CLK_SRC
#define DSPI_MASTER_CLK_FREQ CLOCK_GetFreq(DSPI0_CLK_SRC)
#define EXAMPLE_DSPI_MASTER_PCS_FOR_INIT kDSPI_Pcs0
#define EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0

#define EXAMPLE_DSPI_DEALY_COUNT 0XFFFFFU
#define TRANSFER_SIZE 64U         /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 2000000U /*! Transfer baudrate - 2 megahertz */
#define UART_BAUD_RATE 115000

#define DEMO_UART UART3
#define DEMO_UART_CLKSRC UART3_CLK_SRC
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(UART3_CLK_SRC)
#define DEMO_UART_IRQn UART3_RX_TX_IRQn
#define DEMO_UART_IRQHandler UART3_RX_TX_IRQHandler

#define PORTC_PCR16 (*((volatile uint32_t *)(0x4004B040u)))
#define PORTC_PCR17 (*((volatile uint32_t *)(0x4004B044u)))
#define UART3_C2 (*((volatile uint32_t *)(0x4006D003u)))
#define UART3_D (*((volatile uint32_t *)(0x4006D007u)))

#define DEMO_RING_BUFFER_SIZE 16

uint8_t g_tipString[] = "hello very kind good sir";

uint8_t demoRingBuffer[DEMO_RING_BUFFER_SIZE];
volatile uint16_t txIndex;
volatile uint16_t rxIndex;

void DEMO_UART_IRQHandler(void){
	uint8_t data;
	if((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(DEMO_UART)){
		data = UART_ReadByte(DEMO_UART);
		if(((rxIndex + 1) % DEMO_RING_BUFFER_SIZE) != txIndex){
			demoRingBuffer[rxIndex] = data;
			rxIndex++;
			rxIndex %= DEMO_RING_BUFFER_SIZE;
		}
	}

	#if defined __COTEX_M && (__CORTEX_M == 4U)
		__DSB();
	#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
	uart_config_t config;
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    //CLOCK_EnableClock(kCLOCK_PortC);
    PORTC_PCR17 |= 0x00000300;
    //0x4004B040u |= 0x00000300;
    PORTC_PCR16 |= 0x00000300;
    UART3_C2 |= 0x08;
    UART3_D = 0x56;
    //PORT_SetPinMUX(BOARD_INITPINS_TMR_1588_1_PORT, BOARD_INITPINS_TMR_1588_1_PIN, kPORT_MuxAlt3);

    uint32_t loopCount = 1;

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);

    UART_WriteBlocking(DEMO_UART, g_tipString, sizeof(g_tipString) / sizeof(g_tipString[0]));

    UART_EnableInterrupts(DEMO_UART, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);

    EnableIRQ(DEMO_UART_IRQn);

    while (1){
		//for(int i=0; i<getBlocksSize; i++){
			//PRINTF(" %02X", getBlocksSend[i]);
		//}
    	while((kUART_TxDataRegEmptyFlag & UART_GetStatusFlags(DEMO_UART))){
    	uint8_t data = 99;
    		UART_WriteByte(DEMO_UART, data);
    	}
		uint8_t* returnBlocks2 = getBlocks();
        //while (!isTransferCompleted){
        //}
		for (int i=0; i<20; i++){
			PRINTF(" %02X", returnBlocks2[i]);
		}
        /* Wait for press any key */
        PRINTF("\r\n Press any key to run again\r\n");
        GETCHAR();
        /* Increase loop count to change transmit buffer */
        loopCount++;
    }
}
