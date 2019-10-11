#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"
#include "pin_mux.h"
#include "clock_config.h"

uint8_t* rxData;
dspi_master_config_t masterConfig;
dspi_transfer_t masterXfer;
dspi_master_handle_t g_m_handle;volatile bool isTransferCompleted = false;

//getBlocks data
uint8_t getBlocksSend[] = {174, 193, 32, 2, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getBlocks function
int getBlocksSendSize = sizeof(getBlocksSend);
uint8_t returnBlocks[20];
int getBlocksReturnSize = 20;

//setServos data
uint8_t setServosSend[] = {174, 193, 18, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4], [5], [6], [7] are servo values
int setServosSendSize = sizeof(setServosSend);
uint8_t returnServos[10];
int setServosReturnSize = 10;

//setLED data
uint8_t setLEDSend[] = {174, 193, 20, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4], [5], [6] are rgb values
int setLEDSendSize = sizeof(setLEDSend);
uint8_t returnLED[10];
int setLEDReturnSize = 10;

//setCameraBrightness data
uint8_t setBrightnessSend[] = {174, 193, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] is camera brightness value
int setBrightnessSendSize = sizeof(setBrightnessSend);
uint8_t returnBrightness[10];
int setBrightnessReturnSize = 10;

//getResolution data
uint8_t getResolutionSend[] = {174, 193, 12, 1, 255, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getResolution function
int getResolutionSendSize = sizeof(getResolutionSend);
uint8_t returnResolution[10];
int getResolutionReturnSize = 10;

//getVersion data
uint8_t getVersionSend[] = {174, 193, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getVersion function
int getVersionSendSize = sizeof(getVersionSend);
uint8_t returnVersion[13];
int getVersionReturnSize = 13;

//setLamp data
uint8_t setLampSend[] = {174, 193, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are Lamp values
int setLampSendSize = sizeof(setLampSend);
uint8_t returnLamp[10];
int setLampReturnSize = 10;

//getFPS data
uint8_t getFPSSend[] = {174, 193, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getFPS function
int getFPSSendSize = sizeof(getFPSSend);
uint8_t returnFPS[10];
int getFPSReturnSize = 10;

//getMainFeatures data
uint8_t getMainFeaturesSend[] = {174, 193, 48, 2, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getMainFeatures function
int getMainFeaturesSendSize = sizeof(getMainFeaturesSend);
uint8_t returnMainFeatures[50];
int getMainFeaturesReturnSize = 50;

//setMode data
uint8_t setModeSend[] = {174, 193, 54, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] is mode value
int setModeSendSize = sizeof(setModeSend);
uint8_t returnMode[10];
int setModeReturnSize = 10;

//setNextTurn data
uint8_t setNextTurnSend[] = {174, 193, 58, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are angle
int setNextTurnSendSize = sizeof(setNextTurnSend);
uint8_t returnNextTurn[10];
int setNextTurnReturnSize = 10;

//setDefaultTurn data
uint8_t setDefaultTurnSend[] = {174, 193, 60, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] and [5] are angle
int setDefaultTurnSendSize = sizeof(setDefaultTurnSend);
uint8_t returnDefaultTurn[10];
int setDefaultTurnReturnSize = 10;

//setVector data
uint8_t setVectorSend[] = {174, 193, 56, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};//[4] is vector index/angle
int setVectorSendSize = sizeof(setVectorSend);
uint8_t returnVector[10];
int setVectorReturnSize = 10;

//reverseVector data
uint8_t reverseVectorSend[] = {174, 193, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int reverseVectorSendSize = sizeof(reverseVectorSend);
uint8_t returnReverseVector[10];
int reverseVectorReturnSize = 10;

//getRGB data
uint8_t getRGBSend[] = {174, 193, 112, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//transfer data for getRGB function
int getRGBSendSize = sizeof(getRGBSend);
uint8_t returnRGB[9];
int getRGBReturnSize = 9;

void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData){
    if (status == kStatus_Success){
        __NOP();
    }
    isTransferCompleted = true;
}

uint8_t* genericSPI(uint8_t* txData, uint8_t txDataSize, uint8_t rxDataSize){
	//masterConfig setting
	masterConfig.whichCtar = kDSPI_Ctar0;	masterConfig.ctarConfig.baudRate                      = TRANSFER_BAUDRATE;
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
    DSPI_MasterInit(EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, DSPI_MASTER_CLK_FREQ);
    DSPI_MasterTransferCreateHandle(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, DSPI_MasterUserCallback, NULL);

    /* Start master transfer, send data to slave */
    uint8_t rxDataOut[rxDataSize];
    isTransferCompleted
    = true;
    masterXfer.txData      = txData;//masterTxData
    masterXfer.rxData      = NULL;
    masterXfer.dataSize    = txDataSize;
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
    masterXfer.rxData      = rxDataOut;//masterRxData
    masterXfer.dataSize    = rxDataSize;//Transfer_Size
    masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);
    rxData = rxDataOut;

    return rxData;
}

//start of Pixy2 functions
uint8_t* getBlocks(){
    uint8_t* returnBlocks = genericSPI(getBlocksSend, getBlocksSendSize, getBlocksReturnSize);
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
    uint8_t* returnServos = genericSPI(setServosSend, setServosSendSize, setServosReturnSize);
    return returnServos;
}

uint8_t* setLED(uint8_t r, uint8_t g, uint8_t b){
    setLEDSend[4] = r;
    setLEDSend[5] = g;
    setLEDSend[6] = b;
    uint8_t* returnLED = genericSPI(setLEDSend, setLEDSendSize, setLEDReturnSize);
    return returnLED;
}

uint8_t* setCameraBrightness(uint8_t brightness){
    uint8_t* returnBrightness = genericSPI(setBrightnessSend, setBrightnessSendSize, setBrightnessReturnSize);
    return returnBrightness;
}

uint8_t* getResolution(){
    uint8_t* returnResolution = genericSPI(getResolutionSend, getResolutionSendSize, getResolutionReturnSize);
    return returnResolution;
}

uint8_t* getVersion(){
    uint8_t* returnVersion = genericSPI(getVersionSend, getVersionSendSize, getVersionReturnSize);
    return returnVersion;
}

uint8_t* setLamp(uint8_t upper, uint8_t lower){
    setLampSend[4] = upper;//toggles 2 LED's on top of Pixy2
    setLampSend[5] = lower;//toggles all RGB lights
    uint8_t* returnLamp = genericSPI(setLampSend, setLampSendSize, setLampReturnSize);
    return returnLamp;
}

uint8_t* getFPS(){
    uint8_t* returnFPS = genericSPI(getFPSSend, getFPSSendSize, getFPSReturnSize);
    return returnFPS;
}

uint8_t* getMainFeatures(){
    uint8_t* returnMainFeatures = genericSPI(getMainFeaturesSend, getMainFeaturesSendSize, getMainFeaturesReturnSize);
    return returnMainFeatures;
}

uint8_t* setMode(uint8_t mode){
    setModeSend[4] = mode;//toggles 2 LED's on top of Pixy2
    uint8_t* returnMode = genericSPI(setModeSend, setModeSendSize, setModeReturnSize);
    return returnMode;
}

uint8_t* setNextTurn(uint16_t angle){
    uint8_t anglearray[2];
    anglearray[0]=angle & 0xff;//upper byte
    anglearray[1]=(angle >> 8);//lower byte
    setNextTurnSend[4] = anglearray[1];
    setNextTurnSend[5] = anglearray[0];
    uint8_t* returnNextTurn = genericSPI(setNextTurnSend, setNextTurnSendSize, setNextTurnReturnSize);
    return returnNextTurn;
}

uint8_t* setDefaultTurn(uint16_t angle){
    uint8_t anglearray[2];
    anglearray[0]=angle & 0xff;//upper byte
    anglearray[1]=(angle >> 8);//lower byte
    setDefaultTurnSend[4] = anglearray[1];
    setDefaultTurnSend[5] = anglearray[0];
    uint8_t* returnDefaultTurn = genericSPI(setDefaultTurnSend, setDefaultTurnSendSize, setDefaultTurnReturnSize);
    return returnDefaultTurn;
}

uint8_t* setVector(uint8_t angle){
    setVectorSend[4] = angle;
    uint8_t* returnVector = genericSPI(setVectorSend, setVectorSendSize, setVectorReturnSize);
    return returnVector;
}

uint8_t* reverseVector(){
    uint8_t* returnReverseVector = genericSPI(reverseVectorSend, reverseVectorSendSize, reverseVectorReturnSize);
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
    uint8_t* returnRGB = genericSPI(getRGBSend, getRGBSendSize, getRGBReturnSize);
    return returnRGB;
}
