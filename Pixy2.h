#ifndef Pixy2_H
#define Pixy2_H

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#define EXAMPLE_DSPI_MASTER_BASEADDR SPI0
#define DSPI_MASTER_CLK_SRC DSPI0_CLK_SRC
#define DSPI_MASTER_CLK_FREQ CLOCK_GetFreq(DSPI0_CLK_SRC)
#define EXAMPLE_DSPI_MASTER_PCS_FOR_INIT kDSPI_Pcs0
#define EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0
#define EXAMPLE_DSPI_DEALY_COUNT 0XFFFFFU
#define TRANSFER_SIZE 64U         /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 2000000U /*! Transfer baudrate - 2 megahertz */

void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData);
void genericSPI(uint8_t* txData, uint8_t txDataSize);//returns rxData array
void delay(int a);

//functions for the Pixy2 camera
uint8_t* getBlocks();//function for returning all blocks
uint8_t* setServos(uint16_t s0, uint16_t s1);//values from 0-511, s0 tilt, s1 pan
uint8_t* setLED(uint8_t r, uint8_t g, uint8_t b);//values from 0-255 for colors5
uint8_t* setCameraBrightness(uint8_t brightness);//values from 0-255
uint8_t* getResolution();//returns frame height and width
uint8_t* getVersion();//returns information on version
uint8_t* setLamp(uint8_t upper, uint8_t lower);//lower and upper values 0 or 1
uint8_t* getFPS();//returns frames per second
uint8_t* getMainFeatures();//returns line data: barcodes, vectors, intersections
uint8_t* setMode(uint8_t mode);//sets mode for line tracking
uint8_t* setNextTurn(uint16_t angle);//sets turn angle for camera
uint8_t* setDefaultTurn(uint16_t angle);//sets default turn angle for camera
uint8_t* setVector(uint8_t angle);//vector index function
uint8_t* reverseVector();//reverses vector angle???
uint8_t* getRGB(uint16_t x, uint16_t y, uint8_t saturate);//gets color value at location
#endif
