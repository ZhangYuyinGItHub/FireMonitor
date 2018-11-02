#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"
#include <stdio.h>

void USART1_Config(void);
void USART2_Config(void);
int fputc(int ch, FILE *f);
void UART1_NVIC_Configuration(void);
void USART1_printf(USART_TypeDef *USARTx, uint8_t *Data, ...);

void uart_send_data(USART_TypeDef *uart_idx, uint8_t *buf, uint32_t len);

#endif /* __USART1_H */
