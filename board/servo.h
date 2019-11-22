#ifndef servo_H
#define servo_H

#define FTM0_MOD_VALUE 37500 // Value closest to 65535 without going over to reach 2ms (50Hz) delay (60MHz/2^PS X 50)
#define FTM0_CV_VALUE 18750 // 37500/18750 = 2
#define FTM0_CLK_PRESCALE 5 // Power used to reach MOD value (2^5). Lower = better resolution
#define FTM0_CH1_CLK_SOURCE 1

void PWM_init(void);
void FTM0_IRQHandler(void);
void setFTM0CnV(float a);

#endif
