
#include    <xc.h>

#include    <eeprom.h>
#include    <i2c.h>
#include    <ubxcmd.h>

extern volatile GpsMinimumStruct GpsMinimum;

static void send_eeprom_read() {
    SSPBUF = EEPROM_READ_ADDRESS;
    while(R_nW);
}

static void send_eeprom_write() {
    SSPBUF = EEPROM_WRITE_ADDRESS;
    while(R_nW);
}

void eeprom_single_write(uint8_t data, uint8_t addr) {
    i2c_start();
    send_eeprom_write();
    i2c_wait_for_ack();
    i2c_write_data(addr);
    i2c_wait_for_ack();
    i2c_write_data(data);
    i2c_wait_for_ack();
    i2c_stop();
}

uint8_t eeprom_single_read(uint8_t addr) {

    uint8_t ret;

    i2c_start();
    send_eeprom_write();
    i2c_wait_for_ack();
    i2c_write_data(addr);
    i2c_wait_for_ack();

    i2c_start();
    send_eeprom_read();
    i2c_wait_for_ack();
    i2c_gen_read();
    ret = i2c_get_read_data();
    i2c_send_nack();
    i2c_stop();

    return ret;
}

void eeprom_page_write(uint8_t page, uint8_t* page_data, uint8_t len) {
    uint8_t start_addr = page * 8, i;

    i2c_start();
    send_eeprom_write();
    i2c_wait_for_ack();
    i2c_write_data(start_addr);
    i2c_wait_for_ack();

    for (i=0;i<len;i++) {
        i2c_write_data(page_data[i]);
        i2c_wait_for_ack();
    }

    i2c_stop();

}

void eeprom_seq_read(uint8_t start_addr, uint8_t* return_data, uint8_t len) {

    uint8_t i;

    i2c_start();
    send_eeprom_write();
    i2c_wait_for_ack();
    i2c_write_data(start_addr);
    i2c_wait_for_ack();

    i2c_start();
    send_eeprom_read();
    i2c_wait_for_ack();
    for (i=0;i<len;i++) {
        i2c_gen_read();
        return_data[i] = i2c_get_read_data();
        if (i == (len-1)) i2c_send_nack();
            else i2c_send_ack();
    }
    i2c_stop();

}

void eeprom_write_defaults(void) {

    uint8_t sleep_defaults[4] = {0x00, 0xFF, 0x00};
    uint8_t sp_defaults[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    eeprom_page_write(0, sleep_defaults, 3);
    eeprom_single_write(0x01, 0x0F); // Now the system will know that future powerups are not the first one.
    eeprom_page_write(8, sp_defaults, 7);

}

void eeprom_write_gps_minimum(void) {
    uint8_t gpspage1[8];
    gpspage1[0] = GpsMinimum.year[0];
    gpspage1[1] = GpsMinimum.year[1];
    gpspage1[2] = GpsMinimum.month;
    gpspage1[3] = GpsMinimum.day;
    gpspage1[4] = GpsMinimum.hour;
    gpspage1[5] = GpsMinimum.minute;
    gpspage1[6] = GpsMinimum.second;
    gpspage1[7] = GpsMinimum.num_sat;
    //page 14

    uint8_t gpspage2[8];
    gpspage2[0] = GpsMinimum.lon[0];
    gpspage2[1] = GpsMinimum.lon[1];
    gpspage2[2] = GpsMinimum.lon[2];
    gpspage2[3] = GpsMinimum.lon[3];
    gpspage2[4] = GpsMinimum.lat[0];
    gpspage2[5] = GpsMinimum.lat[1];
    gpspage2[6] = GpsMinimum.lat[2];
    gpspage2[7] = GpsMinimum.lat[3];
    //page 15

    uint8_t gpspage3[8];
    gpspage3[0] = GpsMinimum.height[0];
    gpspage3[1] = GpsMinimum.height[1];
    gpspage3[2] = GpsMinimum.height[2];
    gpspage3[3] = GpsMinimum.height[3];
    gpspage3[4] = GpsMinimum.gspeed[0];
    gpspage3[5] = GpsMinimum.gspeed[1];
    gpspage3[6] = GpsMinimum.gspeed[2];
    gpspage3[7] = GpsMinimum.gspeed[3];
    //page 16

    uint8_t gpspage4[7];
    gpspage4[0] = GpsMinimum.direction[0];
    gpspage4[1] = GpsMinimum.direction[1];
    gpspage4[2] = GpsMinimum.direction[2];
    gpspage4[3] = GpsMinimum.direction[3];
    gpspage4[4] = GpsMinimum.valid_flags;
    gpspage4[5] = GpsMinimum.gps_flags;
    gpspage4[6] = GpsMinimum.time_flags;
    //page 17

    eeprom_page_write(14, gpspage1, 8);
    eeprom_page_write(15, gpspage2, 8);
    eeprom_page_write(16, gpspage3, 8);
    eeprom_page_write(17, gpspage4, 8);
}

void eeprom_write_nrf52_sig_byte(uint8_t sig_byte) {
    eeprom_single_write(sig_byte, EEPROM_NRF52_SIG_BYTE);
}
