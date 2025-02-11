#ifndef __UART_H__
#define __UART_H__

void _uartInit();

void UART_sendString(char *str);
void UART_receiveString(char *buffer, int maxLength);

#endif
