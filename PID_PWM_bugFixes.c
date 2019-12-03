#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "P_Controller.h"
#include "pixy2.h"

int modeValue1;  //0 for cone, 1 for line
float thisU;
float thisE;// send PWM every 20 ms
int turnCounter;
int turnTime;

void PID_Init(){
	thisU = 2812.5;
	modeValue1 = 0;
	turnCounter = 0;
	turnTime = 0;
}

float PID_Controller (int middleError, int lastVal){
	if(modeValue1 == 0){
		turnCounter = 0;
		if(middleError != 0){
			thisE = ((float)middleError); // pixel difference between center of camera and center of pillars, this is error value
			thisU = AvalCone*thisE + midVal;    // actual P calculation
			return thisU;				// this will be new MOD value
		}
		else{
			return lastVal;				// this will be new MOD value
		}
	}
	else{
		if(middleError != 0){
			thisE = ((float)middleError); // pixel difference between center of camera and center of pillars, this is error value
			thisU = AvalLine*thisE + midVal;    // actual P calculation
			return thisU;				// this will be new MOD value
		}
		else{
			return lastVal;				// this will be new MOD value
		}
	}
}

void setModeValue(int mode){
	modeValue1 = mode;
}

float setTurnTime(int way, int lastVal){
	turnTime = 1;
	if(turnCounter > 40){
		if(turnCounter == 40){
			if(way == 1){
				setServos(435, 480);
			}
			else{
				setServos(588, 480);
			}

		}
		if(way == 1){//1 turns right
			return 3750;
		}
		else{// 0 turns left
			return 1875;
		}
		turnCounter++;
	}
	else{
		turnCounter++;
		return lastVal;
	}
}
