#ifndef UART3_d_H
#define UART3_d_H
#include "board.h"

void UART3_init(void);
void UART3_Transmit_Poll(uint8_t data);
void UART3_Transmit_Poll_s(char data[]);
void delay_ms(int milliseconds);
int getArrayLength(uint8_t* array);

#endif
