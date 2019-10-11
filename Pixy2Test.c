#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_uart.h"

int main(void){
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    while (1){
        PRINTF("\r\n Press any key to run\r\n");
        GETCHAR();

        //getBlocks test
		uint8_t* getBlocksData = getBlocks();
        PRINTF("getBlocks test");
		for (int i=0; i<sizeof(getBlocksData); i++){
			PRINTF(" %02X", getBlocksData[i]);
		}

        //setServos test
        uint16_t servo1 = 237;
        uint16_t servo2 = 417;
        uint8_t* setServosData = setServos(servo1, servo2);
        PRINTF("setServos test");
        for (int i=0; i<sizeof(setServosData); i++){
            PRINTF(" %02X", setServosData[i]);
        }

        //setLED test
        uint8_t red = 200;
        uint8_t green = 100;
        uint8_t blue = 150;
        uint8_t* setLEDData = setLED(red, green, blue);
        PRINTF("setLED test");
        for (int i=0; i<sizeof(setLEDData); i++){
            PRINTF(" %02X", setLEDData[i]);
        }

        //setCameraBrightness test
        uint8_t brightness = 138;
        uint8_t* setCameraBrightnessData = setCameraBrightness(brightness);
        PRINTF("setCameraBrightness test");
        for (int i=0; i<sizeof(setCameraBrightnessData); i++){
            PRINTF(" %02X", setCameraBrightnessData[i]);
        }

        //getResolution test
        uint8_t* getResolutionData = getResolution();
        PRINTF("getResolution test");
        for (int i=0; i<sizeof(getResolutionData); i++){
            PRINTF(" %02X", getResolutionData[i]);
        }

        //getVersion test
        uint8_t* getVersionData = getVersion();
        PRINTF("getVersion test");
        for (int i=0; i<sizeof(getVersionData); i++){
            PRINTF(" %02X", getVersionData[i]);
        }

        //setLamp test
        uint8_t upperlight = 1;
        uint8_t lowerlight = 1;
        uint8_t* setLampData = setLamp(upperlight, lowerlight);
        PRINTF("setLamp test");
        for (int i=0; i<sizeof(setLampData); i++){
            PRINTF(" %02X", setLampData[i]);
        }

        //getFPS test
        uint8_t* getFPSData = getFPS();
        PRINTF("getFPS test");
        for (int i=0; i<sizeof(getFPSData); i++){
            PRINTF(" %02X", getFPSData[i]);
        }

        //getMainFeatures test
        uint8_t* getMainFeaturesData = getMainFeatures();
        PRINTF("getMainFeatures test");
        for (int i=0; i<sizeof(getMainFeaturesData); i++){
            PRINTF(" %02X", getMainFeaturesData[i]);
        }

        //setMode test
        uint8_t mode = 2;
        uint8_t* setModeData = setMode(mode);
        PRINTF("setMode test");
        for (int i=0; i<sizeof(setModeData); i++){
            PRINTF(" %02X", setModeData[i]);
        }

        //setNextTurn test
        uint8_t nextTurnAngle = 175;
        uint8_t* setNextTurnData = setNextTurn(nextTurnAngle);
        PRINTF("setNextTurn test");
        for (int i=0; i<sizeof(setNextTurnData); i++){
            PRINTF(" %02X", setNextTurnData[i]);
        }

        //setDefaultTurn test
        uint8_t defaultTurnAngle = 130;
        uint8_t* setDefaultTurnData = setDefaultTurn(defaultTurnAngle);
        PRINTF("setDefaultTurn test");
        for (int i=0; i<sizeof(setDefaultTurnData); i++){
            PRINTF(" %02X", setDefaultTurnData[i]);
        }

        //setVector test
        uint8_t setVectorAngle = 130;
        uint8_t* setVectorData = setVector(setVectorAngle);
        PRINTF("setVector test");
        for (int i=0; i<sizeof(setVectorData); i++){
            PRINTF(" %02X", setVectorData[i]);
        }

        //reverseVector test
        uint8_t* reverseVectorData = reverseVector();
        PRINTF("reverseVector test");
        for (int i=0; i<sizeof(reverseVectorData); i++){
            PRINTF(" %02X", reverseVectorData[i]);
        }

        //getRGB test
        uint16_t x = 175;
        uint16_t y = 115;
        uint8_t saturate = 1;
        uint8_t* getRGBData = getRGB(x, y, saturate);
        PRINTF("getRGB test");
        for (int i=0; i<sizeof(getRGBData); i++){
            PRINTF(" %02X", getRGBData[i]);
        }
    }
}
