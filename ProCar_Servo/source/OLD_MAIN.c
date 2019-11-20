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

//variables for color tracking
int leftConeMidx;
int rightConeMidx;
int coneMidx;
int middleError;
const int pixelMiddle = 157;
uint8_t getBlocksDataLeft[14];
uint8_t getBlocksDataRight[14];
uint8_t* getBlocksData;

//variables for line tracking

uint8_t* getMainFeaturesData;

void getConeBounds(void);
void getLineData(void);
void motorTest(void);
void printArray(uint8_t* array);

int main(void){
	BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    UART3_init();
    PWM_initfun();

    while (1){
        //PRINTF("\r\n Press any key to run\r\n");
        //GETCHAR();
    	getConeBounds();//also getBlocks(), get Left and Right cone data
    	//getLineData();
		delay_ms(700);
		//char data[100];
		//snprintf(data, sizeof(data), "%02X%02X", getBlocksData[6], getBlocksData[7]);
		//UART3_Transmit_Poll_s(data);
    }
}

void getConeBounds(){
	getBlocksData = getBlocks();
	if(getBlocksData[1] == 175){
		for(int i=0; i<63; i++){
			getBlocksData[i] = getBlocksData[i+1];
		}
	}
	for(int i=0; i<14; i++){
		if(getBlocksData[6] == 0X53U){//left
			getBlocksDataLeft[i] = getBlocksData[i+6];
		}
		else if(getBlocksData[20] == 0X53U){//left
			getBlocksDataLeft[i] = getBlocksData[i+20];
		}
		else if(getBlocksData[34] == 0X53U){//left
			getBlocksDataLeft[i] = getBlocksData[i+34];
		}
		else if(getBlocksData[48] == 0X53U){//left
			getBlocksDataLeft[i] = getBlocksData[i+48];
		}
		else{
			getBlocksDataLeft[i] = 0;
		}
	}
	for(int i=0; i<14; i++){
		if(getBlocksData[6] == 0XAU){//left
			getBlocksDataRight[i] = getBlocksData[i+6];
		}
		else if(getBlocksData[20] == 0XAU){//left
			getBlocksDataRight[i] = getBlocksData[i+20];
		}
		else if(getBlocksData[34] == 0XAU){//left
			getBlocksDataRight[i] = getBlocksData[i+34];
		}
		else if(getBlocksData[48] == 0XAU){//left
			getBlocksDataRight[i] = getBlocksData[i+48];
		}
		else{
			getBlocksDataRight[i] = 0;
		}
	}
	leftConeMidx = getBlocksDataLeft[2] + getBlocksDataLeft[3]*256;
	rightConeMidx = getBlocksDataRight[2] + getBlocksDataRight[3]*256;
	coneMidx = (leftConeMidx + rightConeMidx)/2;
	middleError = pixelMiddle - coneMidx; //negative if car is too far to the left
	PRINTF(" %d", leftConeMidx);
	PRINTF(" %d", rightConeMidx);
	PRINTF(" %d", middleError);//will print negative numbers as positive
	if(leftConeMidx > 315){
		leftConeMidx = 0;
		rightConeMidx = 0;
	}
	else if(rightConeMidx > 315){
		leftConeMidx = 0;
		rightConeMidx = 0;
	}
}

void motorTest(){
	for(int i=0; i<10; i++){
		delay_ms(700);
		switch(i){
		case 0:
			setFTM3CnV(.15);//neutral
			break;
		case 1:
			setFTM3CnV(.165);//forward
			break;
		case 2:
			setFTM3CnV(.15);//neutral
			break;
		case 3:
			setFTM3CnV(.14);//reverse
			break;
		case 4:
			setFTM3CnV(.15);//neutral
			break;
		case 5:
			setFTM3CnV(.14);//reverse
			break;
		default:
			setFTM3CnV(.15);//neutral
			break;
		}
	}
}

void getLineData(){
	getMainFeaturesData = getMainFeatures();
	int length = getArrayLength(getMainFeaturesData);
	if(getMainFeaturesData[1] == 175){
		for(int i=0; i<(length-1); i++){
			getMainFeaturesData[i] = getMainFeaturesData[i+1];
		}
	}
	printArray(getMainFeaturesData);
}

void printArray(uint8_t* array){
	PRINTF("\nArray Data:");
	int length = getArrayLength(array);
	for (int i=0; i<length; i++){
		PRINTF(" %02X", array[i]);
	}
}
