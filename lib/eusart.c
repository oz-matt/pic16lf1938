
#include <xc.h>

#include <stdint.h>

#include <eusart.h>
#include <io.h>
#include <error.h>

extern uint8_t tmr0_overflow;

// Write functions

void uart_write(char data) {
    TMR0_START_ISR {
        while(!TRMT); {
            if (tmr0_overflow > 10) hard_lockup_uart();
        }
    } TMR0_STOP
    TXREG = data;
}

void uart_write_string(const char *str) {
  int i;
  for(i=0;str[i]!='\0';i++) {
      uart_write(str[i]);
  }
}
