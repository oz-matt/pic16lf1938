#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void i2c_start(void);
void i2c_stop(void);
void i2c_wait_for_ack(void);
void i2c_gen_read(void);
uint8_t i2c_get_read_data(void);
void i2c_write_data(uint8_t data);
void i2c_send_ack(void);
void i2c_send_nack(void);

#endif  /* I2C_H */
