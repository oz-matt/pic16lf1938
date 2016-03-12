#include    <xc.h>

#include    <stdint.h>

#include    <i2c.h>
#include    <io.h>
#include    <error.h>

extern uint8_t tmr0_overflow;

void i2c_start(void) {
    SEN = 1;
    TMR0_START_ISR {
        while(SEN) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

void i2c_stop(void) {
    PEN = 1;
    TMR0_START_ISR {
        while(PEN) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

void i2c_wait_for_ack(void) {
    TMR0_START_ISR {
        while(ACKSTAT) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

void i2c_gen_read(void) {
    RCEN = 1;
    TMR0_START_ISR {
        while(RCEN) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

uint8_t i2c_get_read_data(void) {
    uint8_t latest_byte;
    latest_byte = SSPBUF;
    return latest_byte;
}

void i2c_write_data(uint8_t data) {
    SSPBUF = data;
    TMR0_START_ISR {
        while(R_nW) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

void i2c_send_ack(void) {
    ACKDT = 0;
    ACKEN = 1;
    TMR0_START_ISR {
        while(ACKEN) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}

void i2c_send_nack(void) {
    ACKDT = 1;
    ACKEN = 1;
    TMR0_START_ISR {
        while(ACKEN) {
            if (tmr0_overflow > 10) hard_lockup(I2C_LOCK);
        }
    } TMR0_STOP
}
