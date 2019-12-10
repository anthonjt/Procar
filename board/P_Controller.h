#ifndef P_Controller_H
#define P_Controller_H

#define Kp 2.89351852 // This value will need to be experimentally determined
#define Ki 2.89351852 // This value will need to be experimentally determined
#define Ts .020
#define AvalLine 28
#define AvalCone 6.6
#define midVal 2812.5

void PID_Init();
float PID_Controller(int middleError, int lastVal);
void setModeValue(int mode);
float setTurnTime(int way, int lastVal);

#endif
