#ifndef P_Controller_H
#define P_Controller_H

#define Kp 2.89351852 // This value will need to be experimentally determined
#define Ki 2.89351852 // This value will need to be experimentally determined
#define Ts .020
#define Aval 11.9

void PID_Init();
float PID_Controller(int middleError);

#endif
