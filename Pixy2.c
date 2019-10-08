#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"

#include "pin_mux.h"
#include "clock_config.h"

uint8_t masterRxData[TRANSFER_SIZE] = {0U};
uint8_t masterTxData[TRANSFER_SIZE] = {0U};
dspi_master_config_t masterConfig;
dspi_transfer_t masterXfer;
uint32_t srcClock_Hz;
dspi_master_handle_t g_m_handle;
volatile bool isTransferCompleted = false;

//getBlocks data
uint8_t getBlocksSend[] = {174, 193, 32, 2, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0};//tranfer data for getBlocks function
int getBlocksSize = sizeof(getBlocksSend);
uint8_t returnBlocks[20];
int getBlocksReturnSize = 20;

//setServos data
uint8_t setServosSend[] = {174, 193, 18, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are servo values
int setServosSize = sizeof(setServosSend);
uint8_t returnServos[10];
int setServosReturnSize = 10;

//setLED data
uint8_t setLEDSend[] = {174, 193, 20, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4], [5], [6] are rgb values
int setLEDSize = sizeof(setServosSend);
uint8_t returnLED[10];
int setLEDReturnSize = 10;

void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData){
    if (status == kStatus_Success){
        __NOP();
    }
    isTransferCompleted = true;
}

uint8_t* getBlocks(){
    masterConfig.whichCtar                                = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate                      = TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame                  = 8U;
    masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;

    masterConfig.whichPcs           = EXAMPLE_DSPI_MASTER_PCS_FOR_INIT;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig.enableContinuousSCK        = false;
    masterConfig.enableRxFifoOverWrite      = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

    srcClock_Hz = DSPI_MASTER_CLK_FREQ;
    DSPI_MasterInit(EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);

    DSPI_MasterTransferCreateHandle(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, DSPI_MasterUserCallback, NULL);
    /* Start master transfer, send data to slave */
    isTransferCompleted
	= true;
    masterXfer.txData      = getBlocksSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = getBlocksSize;
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    /* Wait transfer complete */
    while (!isTransferCompleted){
    }
    /* Delay to wait slave is ready */
    for (int i = 0; i < EXAMPLE_DSPI_DEALY_COUNT; i++){
        __NOP();
    }
    /* Start master transfer, receive data from slave */
    isTransferCompleted    = true;
    masterXfer.txData      = NULL;
    masterXfer.rxData      = returnBlocks;//masterRxData
    masterXfer.dataSize    = getBlocksReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnBlocks;
}

uint8_t* setServos(uint16_t s0, uint16_t s1){
    uint8_t s0array[2];
    uint8_t s1array[2];
    s0array[0]=s0 & 0xff;//upper byte
    s0array[1]=(s0 >> 8);//lower byte
    s1array[0]=s1 & 0xff;//upper byte
    s1array[1]=(s1 >> 8);//lower byte
    setServosSend[4] = s0array[0];
    setServosSend[5] = s0array[1];
    setServosSend[6] = s1array[0];
    setServosSend[7] = s1array[1];
    
    masterConfig.whichCtar                                = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate                      = TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame                  = 8U;
    masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;

    masterConfig.whichPcs           = EXAMPLE_DSPI_MASTER_PCS_FOR_INIT;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig.enableContinuousSCK        = false;
    masterConfig.enableRxFifoOverWrite      = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

    srcClock_Hz = DSPI_MASTER_CLK_FREQ;
    DSPI_MasterInit(EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);

    DSPI_MasterTransferCreateHandle(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, DSPI_MasterUserCallback, NULL);
    /* Start master transfer, send data to slave */
    isTransferCompleted
    = true;
    masterXfer.txData      = setServosSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setServosSize;
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    /* Wait transfer complete */
    while (!isTransferCompleted){
    }
    /* Delay to wait slave is ready */
    for (int i = 0; i < EXAMPLE_DSPI_DEALY_COUNT; i++){
        __NOP();
    }
    /* Start master transfer, receive data from slave */
    isTransferCompleted    = true;
    masterXfer.txData      = NULL;
    masterXfer.rxData      = returnServos;//masterRxData
    masterXfer.dataSize    = setServosReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnServos;
}

uint8_t* setLED(uint8_t r, uint8_t g, uint8_t b){
    setLEDSend[4] = r;
    setLEDSend[5] = g;
    setLEDSend[6] = b;
    
    masterConfig.whichCtar                                = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate                      = TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame                  = 8U;
    masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;

    masterConfig.whichPcs           = EXAMPLE_DSPI_MASTER_PCS_FOR_INIT;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig.enableContinuousSCK        = false;
    masterConfig.enableRxFifoOverWrite      = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

    srcClock_Hz = DSPI_MASTER_CLK_FREQ;
    DSPI_MasterInit(EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);

    DSPI_MasterTransferCreateHandle(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, DSPI_MasterUserCallback, NULL);
    /* Start master transfer, send data to slave */
    isTransferCompleted
    = true;
    masterXfer.txData      = setLEDSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setLEDSize;
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    /* Wait transfer complete */
    while (!isTransferCompleted){
    }
    /* Delay to wait slave is ready */
    for (int i = 0; i < EXAMPLE_DSPI_DEALY_COUNT; i++){
        __NOP();
    }
    /* Start master transfer, receive data from slave */
    isTransferCompleted    = true;
    masterXfer.txData      = NULL;
    masterXfer.rxData      = returnLED;//masterRxData
    masterXfer.dataSize    = setLEDReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnLED;
}
