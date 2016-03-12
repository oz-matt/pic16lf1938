#include    <xc.h>

#include    <stdint.h>
#include    <stdbool.h>
#include    <string.h>
#include    <stdio.h>

#include    <am1805.h>
#include    <i2c.h>
#include    <eusart.h>
#include    <error.h>
#include    <io.h>

extern uint8_t tmr0_overflow;

static void send_am1805_read() {
    SSPBUF = AM1805_READ_ADDRESS;
    TMR0_START_ISR {
        while(R_nW) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

static void send_am1805_write() {
    SSPBUF = AM1805_WRITE_ADDRESS;
    TMR0_START_ISR {
        while(R_nW) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

static void am1805_write_to_addr(uint8_t data, uint8_t addr) {
    i2c_start();
    send_am1805_write();
    i2c_wait_for_ack();
    i2c_write_data(addr);
    i2c_wait_for_ack();
    i2c_write_data(data);
    i2c_wait_for_ack();
    i2c_stop();
}

void am1805_write_defaults(void) {
    am1805_write_to_addr(0x18, 0x11);
    am1805_write_to_addr(0x2D, 0x30);
    am1805_write_to_addr(0x04, 0x10);
    am1805_write_to_addr(0x00, 0x1C);
}

void am1805_start_ctdn_tmr (uint8_t amt_time) {
    am1805_write_to_addr(amt_time, 0x19);
    am1805_write_to_addr(0xC3, 0x18);
    __delay_ms(2000);
    am1805_write_to_addr(0xC3, 0x18);
}

void am1805_initiate_ctdn_sleep (void) {
    am1805_write_to_addr(0x2D, 0x18);
    am1805_write_to_addr(0x00, 0x0F);
    am1805_write_to_addr(0x80, 0x17); // Falling edge of EXTI triggers interrupt (TC)
    PULL_ENSUREPSW_HIGH();
}

void am1805_print_register(uint8_t reg) {

    uint8_t val;

    i2c_start();
    send_am1805_write();
    i2c_wait_for_ack();
    i2c_write_data(reg);
    i2c_wait_for_ack();
    i2c_stop();

    i2c_start();
    send_am1805_read();
    i2c_wait_for_ack();
    i2c_gen_read();
    val = i2c_get_read_data();
    i2c_stop();

    uart_write(val);

}

bool am1805_are_we_asleep(void) {

    uint8_t val;

    i2c_start();
    send_am1805_write();
    i2c_wait_for_ack();
    i2c_write_data(0x17);
    i2c_wait_for_ack();
    i2c_stop();

    i2c_start();
    send_am1805_read();
    i2c_wait_for_ack();
    i2c_gen_read();
    val = i2c_get_read_data();
    i2c_stop();

    if (val & 0x80) return true;
    return false;

}

void am1805_sleep_error(void) {
    // An EXTI interrupt was missed by the am1805.
    // We need to take the am1805 out of sleep.

    // TODO - Write this fxn.
}

static void adjust_pic_time(void) {
    uint8_t tens, ones;
    tens = PicTime.sec;
    ones = tens;
    ones &= 0b00001111;
    tens &= 0b01110000;
    tens = tens >> 4;
    PicTime.sec = ones + (tens * 10);

    tens = PicTime.min;
    ones = tens;
    ones &= 0b00001111;
    tens &= 0b01110000;
    tens = tens >> 4;
    PicTime.min = ones + (tens * 10);

    tens = PicTime.hr;
    ones = tens;
    ones &= 0b00001111;
    tens &= 0b00110000;
    tens = tens >> 4;
    PicTime.hr = ones + (tens * 10);

    tens = PicTime.day;
    ones = tens;
    ones &= 0b00001111;
    tens &= 0b00110000;
    tens = tens >> 4;
    PicTime.day = ones + (tens * 10);

    tens = PicTime.mo;
    ones = tens;
    ones &= 0b00001111;
    tens &= 0b00010000;
    tens = tens >> 4;
    PicTime.mo = ones + (tens * 10);

    tens = PicTime.yr;
    ones = tens;
    ones &= 0b00001111;
    tens &= 0b11110000;
    tens = tens >> 4;
    PicTime.yr = ones + (tens * 10);
}

void am1805_update_pic_time(void) {

    i2c_start();
    send_am1805_write();
    i2c_wait_for_ack();
    i2c_write_data(0x01);
    i2c_wait_for_ack();
    i2c_stop();

    i2c_start();
    send_am1805_read();
    i2c_wait_for_ack();

    i2c_gen_read();
    PicTime.sec = i2c_get_read_data();
    i2c_send_ack();

    i2c_gen_read();
    PicTime.min = i2c_get_read_data();
    i2c_send_ack();

    i2c_gen_read();
    PicTime.hr = i2c_get_read_data();
    i2c_send_ack();

    i2c_gen_read();
    PicTime.day = i2c_get_read_data();
    i2c_send_ack();

    i2c_gen_read();
    PicTime.mo = i2c_get_read_data();
    i2c_send_ack();

    i2c_gen_read();
    PicTime.yr = i2c_get_read_data();
    i2c_send_nack();

    i2c_stop();

    adjust_pic_time();
}

void am1805_print_pic_time (void) {
    char pictime[30];
    sprintf(pictime, "%d:%d:%d %d-%d-%d\r\n", PicTime.hr, PicTime.min, \
        PicTime.sec, PicTime.day, PicTime.mo, PicTime.yr);
    uart_write_string(pictime);
}

void am1805_write_pgm_time(void) {
    i2c_start();
    send_am1805_write();
    i2c_wait_for_ack();
    i2c_write_data(0x01);
    i2c_wait_for_ack();
    i2c_write_data(PGM_TIME_SEC);
    i2c_wait_for_ack();
    i2c_write_data(PGM_TIME_MIN);
    i2c_wait_for_ack();
    i2c_write_data(PGM_TIME_HR);
    i2c_wait_for_ack();
    i2c_write_data(PGM_TIME_DAY);
    i2c_wait_for_ack();
    i2c_write_data(PGM_TIME_MO);
    i2c_wait_for_ack();
    i2c_write_data(PGM_TIME_YR);
    i2c_wait_for_ack();
    i2c_stop();
}

void am1805_write_alarm_pgm_time(void) {
    i2c_start();
    send_am1805_write();
    i2c_wait_for_ack();
    i2c_write_data(0x09);
    i2c_wait_for_ack();
    i2c_write_data(PGM_ALARM_TIME_SEC);
    i2c_wait_for_ack();
    i2c_write_data(PGM_ALARM_TIME_MIN);
    i2c_wait_for_ack();
    i2c_write_data(PGM_ALARM_TIME_HR);
    i2c_wait_for_ack();
    i2c_write_data(PGM_ALARM_TIME_DAY);
    i2c_wait_for_ack();
    i2c_write_data(PGM_ALARM_TIME_MO);
    i2c_wait_for_ack();
    i2c_stop();
}
