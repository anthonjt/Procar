#ifndef DC_Motor_H
#define DC_Motor_H

#define FTM3_MOD_VALUE 37500 // Value closest to 65535 without going over to reach 2ms (50Hz) delay (60MHz/2^PS X 50)
#define FTM3_CLK_PRESCALE 0 // Power used to reach MOD value (2^5). Lower = better resolution
#define FTM3_CH6_CLK_SOURCE 1

void PWM_init_1(void);
void FTM3_IRQHandler(void);

#endif
