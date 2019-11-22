#ifndef DC_Motor_H
#define DC_Motor_H

#define FTM3_MOD_VALUE 37500 // 100 hz
#define FTM3_CLK_PRESCALE 4 // 100 hz
#define FTM3_CH6_CLK_SOURCE 1

void PWM_initfun(void);
void FTM3_IRQHandler(void);
void setFTM3CnV(float a);

#endif
