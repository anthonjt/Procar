#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "board.h"
#include "Pixy2.h"
#include "P_Controller.h"
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
int leftHeight;
int rightHeight;
int turnWay;
int lastPVal;
const int pixelMiddle = 157;
uint8_t getBlocksDataLeft[14];
uint8_t getBlocksDataRight[14];
uint8_t* getBlocksData;

//variables for line tracking

uint8_t* getMainFeaturesData;
uint8_t lineData1[4];
uint8_t lineData2[4];
int lineMiddleTail;
int lineMiddleHead;
int lineMiddle;
int lineErrorTail;
int lineErrorHead;
int lineError;
int turnNow;
int noCone;
int noLine;
const int linexMid = 40;
uint16_t tiltLine = 750;
uint16_t tiltCone = 440;
uint16_t pan = 485;
int modeValue;

float speedVal;

int timeCounter;

void getConeBounds(void);
void getLineData(void);
void motorTest(void);
void printArray(uint8_t* array);
void storeData(void);

int main(void){
	modeValue = 1;//0-cones, 1-accuracy-line, 2-speed-line
	turnNow = 0;
	BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    UART3_init();
    PWM_initfun();
    PWM_init(); // to servo FTM0
    PID_Init();
    setFTM3CnV(.15);
    timeCounter = 0;
    if(modeValue == 0){//cone mode
       	setServos(pan, tiltCone);
       	setLamp(0, 0);
       	speedVal = .1585;
    }
    else if(modeValue == 1){
    	setServos(pan, tiltLine);
    	setLamp(1, 1);
    	speedVal = .1575;
    }
    else{//speed mode
        setServos(pan, tiltLine);
       	setLamp(1, 1);
       	speedVal = .1585;
    }
    delay_ms(500);
    setFTM3CnV(speedVal);
    setModeValue(modeValue);
    lastPVal = 2812.5;
    turnNow = 0;
    while (1){
    	//getConeBounds();
    	//getLineData();
    	if(modeValue == 0){//Cones
    		if(turnNow == 0){
    		}
    		else{

    		}
    		getConeBounds();
    		if(leftHeight > 135 || rightHeight > 135){
    			turnNow = 1;
    			setFTM0CnV(setTurnTime(turnWay, lastPVal));
    		}
    		else{
    			turnNow = 0;
    			setServos(pan, tiltCone);
    			lastPVal = PID_Controller(middleError, lastPVal);
    			setFTM0CnV(lastPVal);
    		}
    	}
    	else{//Line
    		getLineData();
    		if(lineMiddleTail == 0 || lineMiddleHead == 0){

    		}
    		else{
    			lastPVal = PID_Controller(lineErrorTail, lastPVal);
    			setFTM0CnV(lastPVal);
    		}
    	}
    	storeData();
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
		if(getBlocksData[6] == 0XBU){//left
			getBlocksDataLeft[i] = getBlocksData[i+6];
		}
		else if(getBlocksData[20] == 0XBU){//left
			getBlocksDataLeft[i] = getBlocksData[i+20];
		}
		else if(getBlocksData[34] == 0XBU){//left
			getBlocksDataLeft[i] = getBlocksData[i+34];
		}
		else if(getBlocksData[48] == 0XBU){//left
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
	middleError = coneMidx - pixelMiddle; //negative if car is too far to the left
	rightHeight = getBlocksDataRight[8];
	leftHeight = getBlocksDataLeft[8];
	PRINTF(" %d", leftConeMidx);
	PRINTF(" %d", rightConeMidx);
	PRINTF(" %d", middleError);//will print negative numbers as positive
	if(leftConeMidx > 315 || leftConeMidx == 0){
		leftConeMidx = 0;
		rightConeMidx = 0;
		leftHeight = 255;
		rightHeight = 255;
	}
	else if(rightConeMidx > 315 || rightConeMidx == 0){
		leftConeMidx = 0;
		rightConeMidx = 0;
		leftHeight = 255;
		rightHeight = 255;
	}
	if(leftConeMidx == 0 || rightConeMidx == 0){
	}
	else{
		if(leftConeMidx < rightConeMidx){
			turnWay = 0;
		}
		else if(leftConeMidx > rightConeMidx){
			turnWay = 1;
		}
	}
}

void motorTest(){
	for(int i=0; i<6; i++){
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
	if(getMainFeaturesData[1] == 175){
		for(int i=0; i<49; i++){
			getMainFeaturesData[i] = getMainFeaturesData[i+1];
		}
	}
	for(int i=0; i<50; i++){
		uint8_t index = getMainFeaturesData[i];
		switch(i){
		case 8:
			lineData1[0] = index;
			break;
		case 9:
			lineData1[1] = index;
			break;
		case 10:
			lineData1[2] = index;
			break;
		case 11:
			lineData1[3] = index;
			break;
		case 16:
			lineData2[0] = index;
			break;
		case 17:
			lineData2[1] = index;
			break;
		case 18:
			lineData2[2] = index;
			break;
		case 19:
			lineData2[3] = index;
			break;
		default:
			break;
		}
	}
	lineMiddleTail = lineData1[0];
	lineMiddleHead = lineData1[2];
	lineMiddle = (int)((lineMiddleHead + lineMiddleTail)/2);
	if(lineMiddle < 2 || lineMiddle > 76){
		lineMiddleTail = 0;
		lineMiddleHead = 0;
	}
	lineError = lineMiddle - linexMid;
	lineErrorTail = lineMiddleTail - linexMid;
	lineErrorHead = lineMiddleHead - linexMid;
	PRINTF("\nArray Data:");
	PRINTF(" %d", lineMiddleTail);
	PRINTF(" %d", lineMiddleHead);
	PRINTF(" %d", lineErrorTail);
	PRINTF(" %d", lineErrorHead);
}

void printArray(uint8_t* array){
	PRINTF("\nArray Data:");
	int length = getArrayLength(array);
	for (int i=0; i<length; i++){
		PRINTF(" %02X", array[i]);
	}
}

void storeData(){
	if(timeCounter == 230){
	    timeCounter = 0;
		int Speed = (int)((2000)*(speedVal - .15));
		int turnAngle = (int)(.096*(lastPVal-2812.5));
		if(turnAngle > 90){
			turnAngle = 90;
		}
		else if(turnAngle < -90){
			turnAngle = -90;
		}
		else if(turnAngle < 91 && turnAngle > -91){

		}
		else{
			turnAngle = 0;
		}
		char modeToSend[9];
		if(modeValue == 0){
			snprintf(modeToSend, sizeof(modeToSend), "Cones");
		}
		else if(modeValue == 1){
			snprintf(modeToSend, sizeof(modeToSend), "Accuracy");
		}
		else{
			snprintf(modeToSend, sizeof(modeToSend), "Speed");
		}
		char data1[200];
		snprintf(data1, sizeof(data1), "Speed=%d&Turn_Angle=%d&Mode=%s", Speed, turnAngle, modeToSend);
		UART3_Transmit_Poll_s(data1);
	}
	else{
		timeCounter++;
   	}
}
