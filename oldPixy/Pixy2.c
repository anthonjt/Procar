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
uint8_t getBlocksSend[] = {174, 193, 32, 2, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getBlocks function
int getBlocksSize = sizeof(getBlocksSend);
uint8_t returnBlocks[20];
int getBlocksReturnSize = 20;

//setServos data
uint8_t setServosSend[] = {174, 193, 18, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4], [5], [6], [7] are servo values
int setServosSize = sizeof(setServosSend);
uint8_t returnServos[10];
int setServosReturnSize = 10;

//setLED data
uint8_t setLEDSend[] = {174, 193, 20, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4], [5], [6] are rgb values
int setLEDSize = sizeof(setLEDSend);
uint8_t returnLED[10];
int setLEDReturnSize = 10;

//setCameraBrightness data
uint8_t setBrightnessSend[] = {174, 193, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] is camera brightness value
int setBrightnessSize = sizeof(setBrightnessSend);
uint8_t returnBrightness[10];
int setBrightnessReturnSize = 10;

//getResolution data
uint8_t getResolutionSend[] = {174, 193, 12, 1, 255, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getResolution function
int getResolutionSize = sizeof(getResolutionSend);
uint8_t returnResolution[10];
int getResolutionReturnSize = 10;

//getVersion data
uint8_t getVersionSend[] = {174, 193, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getVersion function
int getVersionSize = sizeof(getVersionSend);
uint8_t returnVersion[13];
int getVersionReturnSize = 13;

//setLamp data
uint8_t setLampSend[] = {174, 193, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are Lamp values
int setLampSize = sizeof(setLampSend);
uint8_t returnLamp[10];
int setLampReturnSize = 10;

//getFPS data
uint8_t getFPSSend[] = {174, 193, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getFPS function
int getFPSSize = sizeof(getFPSSend);
uint8_t returnFPS[10];
int getFPSReturnSize = 10;

//getMainFeatures data
uint8_t getMainFeaturesSend[] = {174, 193, 48, 2, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getMainFeatures function
int getMainFeaturesSize = sizeof(getMainFeaturesSend);
uint8_t returnMainFeatures[50];
int getMainFeaturesReturnSize = 50;

//setMode data
uint8_t setModeSend[] = {174, 193, 54, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] is mode value
int setModeSize = sizeof(setModeSend);
uint8_t returnMode[10];
int setModeReturnSize = 10;

//setNextTurn data
uint8_t setNextTurnSend[] = {174, 193, 58, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are angle
int setNextTurnSize = sizeof(setNextTurnSend);
uint8_t returnNextTurn[10];
int setNextTurnReturnSize = 10;

//setDefaultTurn data
uint8_t setDefaultTurnSend[] = {174, 193, 60, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are angle
int setDefaultTurnSize = sizeof(setDefaultTurnSend);
uint8_t returnDefaultTurn[10];
int setDefaultTurnReturnSize = 10;

//setVector data
uint8_t setVectorSend[] = {174, 193, 56, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] is vector index/angle
int setVectorSize = sizeof(setVectorSend);
uint8_t returnVector[10];
int setVectorReturnSize = 10;

//reverseVector data
uint8_t reverseVectorSend[] = {174, 193, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int reversVectorSize = sizeof(reverseVectorSend);
uint8_t returnReverseVector[10];
int reverseVectorReturnSize = 10;

//getRGB data
uint8_t getRGBSend[] = {174, 193, 112, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getRGB function
int getRGBSize = sizeof(getRGBSend);
uint8_t returnRGB[9];
int getRGBReturnSize = 9;

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
    setServosSend[4] = s0array[1];
    setServosSend[5] = s0array[0];
    setServosSend[6] = s1array[1];
    setServosSend[7] = s1array[0];
    
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

uint8_t* setCameraBrightness(uint8_t brightness){
    setBrightnessSend[4] = brightness;
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
    masterXfer.txData      = setBrightnessSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setBrightnessSize;
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
    masterXfer.rxData      = returnBrightness;//masterRxData
    masterXfer.dataSize    = setBrightnessReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnBrightness;
}

uint8_t* getResolution(){
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
    masterXfer.txData      = getResolutionSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = getReslutionSize;
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
    masterXfer.rxData      = returnResolution;//masterRxData
    masterXfer.dataSize    = getResolutionReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnResolution;
}

uint8_t* getVersion(){
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
    masterXfer.txData      = getVersionSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = getVersionSize;
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
    masterXfer.rxData      = returnVersion;//masterRxData
    masterXfer.dataSize    = getVersionReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnVersion;
}

uint8_t* setLamp(uint8_t upper, uint8_t lower){
    setLampSend[4] = upper;//toggles 2 LED's on top of Pixy2
    setLampSend[5] = lower;//toggles all RGB lights
    
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
    masterXfer.txData      = setLampSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setLampSize;
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
    masterXfer.rxData      = returnLamp;//masterRxData
    masterXfer.dataSize    = setLampReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnLamp;
}

uint8_t* getFPS(){
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
    masterXfer.txData      = getFPSSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = getFPSSize;
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
    masterXfer.rxData      = returnFPS;//masterRxData
    masterXfer.dataSize    = getFPSReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnFPS;
}

uint8_t* getMainFeatures(){    masterConfig.whichCtar                                = kDSPI_Ctar0;
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
    masterXfer.txData      = getMainFeaturesSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = getMainFeaturesSize;
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
    masterXfer.rxData      = returnMainFeatures;//masterRxData
    masterXfer.dataSize    = getMainFeaturesReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnMainFeatures;
}

uint8_t* setMode(uint8_t mode){
    setModeSend[4] = mode;//toggles 2 LED's on top of Pixy2
    
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
    masterXfer.txData      = setModeSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setModeSize;
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
    masterXfer.rxData      = returnMode;//masterRxData
    masterXfer.dataSize    = setModeReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnMode;
}

uint8_t* setNextTurn(uint16_t angle){
    
    uint8_t anglearray[2];
    anglearray[0]=angle & 0xff;//upper byte
    anglearray[1]=(angle >> 8);//lower byte
    setNextTurnSend[4] = anglearray[1];
    setNextTurnSend[5] = anglearray[0];
    
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
    masterXfer.txData      = setNextTurnSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setNextTurnSize;
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
    masterXfer.rxData      = returnNextTurn;//masterRxData
    masterXfer.dataSize    = setNextTurnReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnNextTurn;
}

uint8_t* setDefaultTurn(uint16_t angle){
    
    uint8_t anglearray[2];
    anglearray[0]=angle & 0xff;//upper byte
    anglearray[1]=(angle >> 8);//lower byte
    setDefaultTurnSend[4] = anglearray[1];
    setDefaultTurnSend[5] = anglearray[0];
    
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
    masterXfer.txData      = setDefaultTurnSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setDefaultTurnSize;
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
    masterXfer.rxData      = returnDefaultTurn;//masterRxData
    masterXfer.dataSize    = setDefaultTurnReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnDefaultTurn;
}

uint8_t* setVector(uint8_t angle){
    setVectorSend[4] = angle;
    
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
    masterXfer.txData      = setVectorSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = setVectorSize;
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
    masterXfer.rxData      = returnVector;//masterRxData
    masterXfer.dataSize    = setVectorReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnVector;
}

uint8_t* reverseVector(){
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
    masterXfer.txData      = reverseVectorSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = reverseVectorSize;
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
    masterXfer.rxData      = returnReverseVector;//masterRxData
    masterXfer.dataSize    = reverseVectorReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnReverseVector;
}

uint8_t* getRGB(uint16_t x, uint16_t y, uint8_t saturate){
    uint8_t xarray[2];
    uint8_t yarray[2];
    xarray[0]=x & 0xff;//upper byte
    xarray[1]=(x >> 8);//lower byte
    yarray[0]=y & 0xff;//upper byte
    yarray[1]=(y >> 8);//lower byte
    getRGBSend[4] = xarray[1];
    getRGBSend[5] = xarray[0];
    getRGBSend[6] = yarray[1];
    getRGBSend[7] = yarray[0];
    getRGBSend[8] = saturate;
    
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
    masterXfer.txData      = getRGBSend;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = getRGBSize;
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
    masterXfer.rxData      = returnRGB;//masterRxData
    masterXfer.dataSize    = getRGBReturnSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    return returnRGB;
}
