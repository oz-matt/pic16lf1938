#include <xc.h>

#include <brex.h>
#include <am1805.h>
#include <eeprom.h>
#include <io.h>
#include <cti.h>
#include <eusart.h>

#define _XTAL_FREQ 2000000

void first_pwrup_calls(void) {
    am1805_write_pgm_time();
    am1805_write_defaults();
    eeprom_write_defaults();
}

void let_nrf_update_eeprom(void) {
    //PULL_EEPROM_MASTER_SEL_HIGH();

    eeprom_write_nrf52_sig_byte(UPDATE_EEPROM_FROM_SD);

    ENABLE_NRF52();

    DEBUG("Waiting for NRF52 to finish...\r\n");

    while (!is_nrfcomplete_high()); // After this, the EEPROM paramters will be up to date.

    DEBUG("...NRF52 has finished.\r\n");

    DISABLE_NRF52();
}

void reset_instruction_ptr(void) {
    eeprom_single_write(0x50, 0x4F);
    DEBUG("Resetting instruction ptr to 0x50.\r\n");
}

static void adjusted_start (void) {
    cti_instruction_loop();
}

void sleep_start (void) {

    uint8_t sleep_vals[4] = {0};

    eeprom_seq_read(0x00, sleep_vals, 3);
    //sleep_vals[0] = 0x00
    //sleep_vals[1] = 0x01
    //sleep vals[2] = 0x02

    if (sleep_vals[0] > 1) {
        sleep_vals[0] -= 1;
        eeprom_single_write(sleep_vals[0], 0x00);
        am1805_start_ctdn_tmr(255);
        am1805_initiate_ctdn_sleep();
        while(1); //Should be put to sleep here
    } else if (sleep_vals[0] == 1) {
        eeprom_single_write(0, 0x00);
        am1805_start_ctdn_tmr(sleep_vals[2]);
        am1805_initiate_ctdn_sleep();
        while(1); //Should be put to sleep here
    } else if (sleep_vals[0] == 0) {
        adjusted_start();
    }
}
