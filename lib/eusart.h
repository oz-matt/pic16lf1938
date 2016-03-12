#ifndef EUSART_H
#define EUSART_H

#define _XTAL_FREQ 2000000

void uart_write(char data);
void uart_write_string(const char *str);

#endif  /* EUSART_H */
