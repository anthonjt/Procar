#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"

#include "pin_mux.h"
#include "clock_config.h"
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
//masterTxData[100] = {};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* DSPI user callback */
//void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
//uint8_t masterRxData[TRANSFER_SIZE] = {0U};
////uint8_t masterTxData[TRANSFER_SIZE] = {0U};
//dspi_master_config_t masterConfig;
//dspi_transfer_t masterXfer;
////uint8_t getBlocksSend[] = {174, 193, 32, 2, 255, 255};//tranfer data for getBlocks function
////int getBlocksSize = sizeof(getBlocksSend);
//uint8_t returnBlocks[20];
////int getBlocksReturnSize = 20;
//
//uint32_t srcClock_Hz;
//
//dspi_master_handle_t g_m_handle;
//volatile bool isTransferCompleted = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

//void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData){
//    if (status == kStatus_Success){
//        __NOP();
//    }
//    isTransferCompleted = true;
//}

//uint8_t* getBlocks();

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    uint32_t loopCount = 1;

    while (1){
		//for(int i=0; i<getBlocksSize; i++){
			//PRINTF(" %02X", getBlocksSend[i]);
		//}
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

//uint8_t* getBlocks(){
//    masterConfig.whichCtar                                = kDSPI_Ctar0;
//    masterConfig.ctarConfig.baudRate                      = TRANSFER_BAUDRATE;
//    masterConfig.ctarConfig.bitsPerFrame                  = 8U;
//    masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveHigh;
//    masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseFirstEdge;
//    masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
//    masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 1000000000U / TRANSFER_BAUDRATE;
//    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 1000000000U / TRANSFER_BAUDRATE;
//    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;
//
//    masterConfig.whichPcs           = EXAMPLE_DSPI_MASTER_PCS_FOR_INIT;
//    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
//
//    masterConfig.enableContinuousSCK        = false;
//    masterConfig.enableRxFifoOverWrite      = false;
//    masterConfig.enableModifiedTimingFormat = false;
//    masterConfig.samplePoint                = kDSPI_SckToSin0Clock;
//
//    srcClock_Hz = DSPI_MASTER_CLK_FREQ;
//    DSPI_MasterInit(EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);
//
//    DSPI_MasterTransferCreateHandle(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, DSPI_MasterUserCallback, NULL);
//    /* Start master transfer, send data to slave */
//    isTransferCompleted
//	= true;
//    masterXfer.txData      = getBlocksSend;//masterTxData
//    masterXfer.rxData      = NULL;
//    masterXfer.dataSize    = getBlocksSize;
//    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
//    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
//    /* Wait transfer complete */
//    while (!isTransferCompleted){
//    }
//    /* Delay to wait slave is ready */
//    for (int i = 0; i < EXAMPLE_DSPI_DEALY_COUNT; i++){
//        __NOP();
//    }
//    /* Start master transfer, receive data from slave */
//    isTransferCompleted    = true;
//    masterXfer.txData      = NULL;
//    masterXfer.rxData      = returnBlocks;//masterRxData
//    masterXfer.dataSize    = getBlocksReturnSize;//Transfer_Size
//    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
//    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
//    return returnBlocks;
//}
