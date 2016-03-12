#include    <xc.h>

#include    <stdint.h>
#include    <stdio.h>

#include    <cti.h>
#include    <eeprom.h>
#include    <io.h>
#include    <ubxcmd.h>
#include    <am1805.h>
#include    <eusart.h>

#define     _XTAL_FREQ  2000000

static void i3gnav(void) {
    DEBUG("Running instruction '3GNAV'.\r\n");
    ENABLE_GPS();
    __delay_ms(2000);
    ubx_silence_nmea();
    __delay_ms(2000);

    DEBUG("Waiting for GPS fix...\r\n");
    while (!have_3d_fix()) {
        __delay_ms(3000);
    }
    DEBUG("... Got GPS fix.\r\n");

    update_gps_fix_data();

    eeprom_write_gps_minimum();

    eeprom_write_nrf52_sig_byte(SIG_BYTE_3GNAV);

    ENABLE_NRF52();
    ENABLE_EHS6();

    // NRF52 will control power flow for EHS6. When it is all
    // done, it will pull NRFCOMPLETE high.
    DEBUG("Waiting for NRF52 to complete '3GNAV' code...\r\n");
    while (!is_nrfcomplete_high());
    DEBUG("... NRF52 has completed '3GNAV' code\r\n");
    DISABLE_NRF52();
    DISABLE_EHS6();

}

static void idsn(uint16_t n) {
    uint8_t quotient = 0;
	uint8_t remainder = 0;

	while (n > 255) {
		quotient++;
		n -= 255;
	}

	remainder = n;

    char debug[20];
    sprintf(debug, "n=%dq=%dr=%d", n, quotient, remainder);
    DEBUG("Preparing to sleep with");
    DEBUG(debug);
    DEBUG("as sleep vars.\r\n");

    if (quotient > 0) {
        eeprom_single_write(quotient, 0x00); //Number of 255 minute deep sleep cycles left.
        eeprom_single_write(remainder, 0x02);
        DEBUG("Starting CTDN timer at 255.\r\n");
        am1805_start_ctdn_tmr(255);
    } else {
        eeprom_single_write(0, 0x00); // Since this is zero, when pic16 wakes up next it will know that it needs to do a full wakeup.
        eeprom_single_write(remainder, 0x02);
        DEBUG("Starting CTDN timer at r.\r\n");
        am1805_start_ctdn_tmr(remainder);
    }
    am1805_initiate_ctdn_sleep();
    while (1); // Now we wait for the AM1805 to turn us off. When we wake up,
            // we read 0x00. If it's zero, do a full wakeup. If it's 1, copy 0x02 into a start_ctdn_timer, and decrement 0x00.
            // If it's 2 or more, copy 255 into start_ctdn_timer, and decrement 0x00.
}

void cti_instruction_loop(void) {
    while(1) {
        DEBUG("Getting next CTI instruction...\r\n");
        uint8_t next_instruction_address = eeprom_single_read(0x4F);
        uint8_t instruction = eeprom_single_read(next_instruction_address);
        uint8_t low_n = eeprom_single_read(next_instruction_address + 1);
        uint8_t high_n = eeprom_single_read(next_instruction_address + 2);
        uint16_t n = ((uint16_t)(high_n << 8) | low_n);
        switch (instruction) {
            case I3GNAV:
                i3gnav();
                eeprom_single_write(next_instruction_address + 3, 0x4F);
            break;
            case IDSn:
                idsn(n);
                eeprom_single_write(next_instruction_address + 3, 0x4F);
            break;

            case 0: //End of instruction pattern, loop back to 0x50
                DEBUG("End of instruction pattern.\r\n");
                reset_instruction_ptr();
            break;
        }
    }
}
