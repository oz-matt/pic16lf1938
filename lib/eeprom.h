#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

/* ------------- EEPROM LAYOUT ---------------- */

#define     EEPROM_NRF52_SIG_BYTE       0x0A
#define     EEPROM_FIRST_PWRUP_ADDR     0x0F

#define     EEPROM_INSTRUCTION_MODE     0x40

/* -------------------------------------------- */

/* ------- EEPROM SIG BYTE INSTRUCTIONS --------*/

#define     SIG_BYTE_3GNAV              0x03

#define     UPDATE_EEPROM_FROM_SD       0x0F

/* -------------------------------------------- */


#define     EEPROM_WRITE_ADDRESS        0xA0
#define     EEPROM_READ_ADDRESS         0xA1

void eeprom_single_write(uint8_t data, uint8_t addr);
uint8_t eeprom_single_read(uint8_t addr);
void eeprom_page_write(uint8_t page, uint8_t* page_data, uint8_t len);
void eeprom_seq_read(uint8_t start_addr, uint8_t* return_data, uint8_t len);
void eeprom_write_defaults(void);
void eeprom_write_gps_minimum(void);
void eeprom_write_nrf52_sig_byte(uint8_t sig_byte);

#endif  /* EEPROM_H */
