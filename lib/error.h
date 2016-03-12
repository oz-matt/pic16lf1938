#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

#define     HARD_LOCK       1
#define     I2C_LOCK        2
#define     UART_LOCK       3
#define     UNIMP_LOCK      4
#define     GPS_COMM_LOCK   5

void hard_lockup (uint8_t err_num);
void hard_lockup_uart(void);

#endif  /* ERROR_H */
