#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "P_Controller.h"

float lastU;// start servo at 90 degrees
float thisU;
float lastE;
float thisE;// send PWM every 20 ms

void PID_Init(){
	lastU = 2812.5;
	lastE = 0;
}

float PID_Controller (int middleError){
	if(middleError != 0){
		thisE = ((float)middleError); // pixel difference between center of camera and center of pillars, this is error value
		//thisU = lastU + (Kp + Ki*(2/Ts))*thisE + (-Kp+Ki*(2/Ts))*lastE;    // actual P calculation
		thisU = lastU + Aval*thisE;    // actual P calculation
		lastU = thisU; 				// set lastU to thisU for proceeding calculation
		lastE = thisE; 				// set lastE to thisE for proceeding calculation
		return thisU;				// this will be new MOD value
	}
	else{
		return thisU;				// this will be new MOD value

	}
}
