#include    <xc.h>

#include    <error.h>
#include    <io.h>
#include    <eusart.h>
#include    <string.h>

#define _XTAL_FREQ  2000000

static void error_blaster (uint8_t err_num) {
    char str[20];
    switch (err_num) {
        case HARD_LOCK:
            strcpy(str, "1 - Hard Lock\r\n");
            uart_write_string(str);
            break;
        case I2C_LOCK:
            strcpy(str, "2 - I2C Lock\r\n");
            uart_write_string(str);
            break;
        case UNIMP_LOCK:
            strcpy(str, "4 - Unimp Lock\r\n");
            uart_write_string(str);
            break;
        case GPS_COMM_LOCK:
            strcpy(str, "5 - GPS Comm Lock\r\n");
            uart_write_string(str);
            break;
    }
}

// If we get to this function, we will blink the red led
// the same number of times as the error number, and output
// the error type on the UART.

void hard_lockup (uint8_t err_num) {

    uint8_t i;

    set_led_duty(RED, 20);

    __delay_ms(1000);

    while(1) {

        for (i=0;i<err_num;i++) {
            set_led_duty(RED, 80);
            __delay_ms(80);
            set_led_duty(RED, 20);
            __delay_ms(220);
        }
        error_blaster(err_num);
        __delay_ms(4000);
    }
}

void hard_lockup_uart(void) {
    uint8_t i;

    set_led_duty(RED, 20);

    __delay_ms(1000);

    while(1) {

        for (i=0;i<3;i++) {
            set_led_duty(RED, 80);
            __delay_ms(80);
            set_led_duty(RED, 20);
            __delay_ms(220);
        }
        __delay_ms(4000);
    }
}
