#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_uart.h"
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

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    while (1){
        PRINTF("\r\n Press any key to run\r\n");
        GETCHAR();
        
        //getBlocks test
		uint8_t* getBlocksData = getBlocks();
        PRINTF("getBlocks test");
		for (int i=0; i<20; i++){
			PRINTF(" %02X", getBlocksData[i]);
		}
        
        //setServos test
        uint16_t servo1 = 237;
        uint16_t servo2 = 417
        uint8_t* setServosData = setServos(servo1, servo2);
        PRINTF("setServos test");
        for (int i=0; i<20; i++){
            PRINTF(" %02X", setServosData[i]);
        }
        
        //setLED test
        uint8_t red = 200;
        uint8_t green = 100;
        uint8_t blue = 150;
        uint8_t* setLEDData = setLED(red, green, blue);
        PRINTF("setLED test");
        for (int i=0; i<20; i++){
            PRINTF(" %02X", setLEDData[i]);
        }
        
        //setCameraBrightness test
        uint8_t brightness = 130;
        uint8_t* setCameraBrightnessData = setCameraBrightness(brightness);
        PRINTF("setCameraBrightness test");
        for (int i=0; i<20; i++){
            PRINTF(" %02X", setCameraBrightnessData[i]);
        }
    }
}
