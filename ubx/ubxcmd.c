
#include <xc.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <ubxcmd.h>
#include <eusart.h>
#include <string.h>
#include <io.h>
#include <error.h>

#define _XTAL_FREQ  2000000

volatile UbxReceiverStruct UbxReceiver;
volatile GpsMinimumStruct GpsMinimum;

extern uint8_t tmr0_overflow;

bool do_we_have_3d_fix(void) {
    //ubx_send_and_recv(ubx_poll_status, 1);
    return false;
}

void reset_ubx_struct(void) {
    memset(UbxReceiver.buf, 0, sizeof(UbxReceiver.buf));
    UbxReceiver.idx = 0;
}

//str[0] must always be the length of the command!!!
void ubx_send_str(char *str) {
    int i;
    for (i=1;i<=(str[0]);i++) {
        uart_write(str[i]);
    }
}

//This function will fill up 'UbxReceiver.buf' with the reply.
//The length of the response will be in 'UbxReceiver.idx'

bool ubx_send_and_recv(char *ubx_cmd, uint8_t response_time, uint8_t expected_len) {

    uint8_t i;

    BEGIN_UART_LISTEN {

        ubx_send_str(ubx_cmd);
        for (i=0;i<response_time;i++) {
            __delay_ms(50);
        }
        TMR0_START_ISR {
            while((tmr0_overflow<40) && (UbxReceiver.idx != expected_len));
        } TMR0_STOP

    } END_UART_LISTEN

    if (UbxReceiver.idx == expected_len) return true;

    return false;

}

void ubx_silence_nmea (void) {
    ubx_send_str(ubx_disable_gxgga);
    ubx_send_str(ubx_disable_gxgll);
    ubx_send_str(ubx_disable_gxgsa);
    ubx_send_str(ubx_disable_gxgsv);
    ubx_send_str(ubx_disable_gxrmc);
    ubx_send_str(ubx_disable_gxvtg);

    __delay_ms(2000);

    CREN = 0;
    CREN = 1;
}

static bool eval_ubx_buf_for_fix(void) {
    //UbxReceiver.buf contains the correct reply string. For example:
    //B5 62 01 03 10 00 40 EA 99 OE 00 4C 00 60 00 00 00 00 9F C3 0D 00 00 BA
    if (UbxReceiver.buf[10] == 0x03) return true;
    return false;
}

static void get_pvt_str (void) {
    uint8_t err_ctr = 0;
    while (!ubx_send_and_recv(ubx_poll_pvt, 5, 100)) {
        if (err_ctr > 2) hard_lockup(GPS_COMM_LOCK);
        err_ctr++;
        __delay_ms(100);
    }
}

static void sort_pvt_data_into_gps_struct(void) {
    GpsMinimum.year[0] = UbxReceiver.buf[10];
    GpsMinimum.year[1] = UbxReceiver.buf[11];

    GpsMinimum.month = UbxReceiver.buf[12];
    GpsMinimum.day = UbxReceiver.buf[13];
    GpsMinimum.hour = UbxReceiver.buf[14];
    GpsMinimum.minute = UbxReceiver.buf[15];
    GpsMinimum.second = UbxReceiver.buf[16];

    GpsMinimum.valid_flags = UbxReceiver.buf[17];
    GpsMinimum.gps_flags = UbxReceiver.buf[27];
    GpsMinimum.time_flags = UbxReceiver.buf[28];

    GpsMinimum.num_sat = UbxReceiver.buf[29];

    GpsMinimum.lon[0] = UbxReceiver.buf[30];
    GpsMinimum.lon[1] = UbxReceiver.buf[31];
    GpsMinimum.lon[2] = UbxReceiver.buf[32];
    GpsMinimum.lon[3] = UbxReceiver.buf[33];

    GpsMinimum.lat[0] = UbxReceiver.buf[34];
    GpsMinimum.lat[1] = UbxReceiver.buf[35];
    GpsMinimum.lat[2] = UbxReceiver.buf[36];
    GpsMinimum.lat[3] = UbxReceiver.buf[37];

    GpsMinimum.height[0] = UbxReceiver.buf[38];
    GpsMinimum.height[1] = UbxReceiver.buf[39];
    GpsMinimum.height[2] = UbxReceiver.buf[40];
    GpsMinimum.height[3] = UbxReceiver.buf[41];

    GpsMinimum.gspeed[0] = UbxReceiver.buf[66];
    GpsMinimum.gspeed[1] = UbxReceiver.buf[67];
    GpsMinimum.gspeed[2] = UbxReceiver.buf[68];
    GpsMinimum.gspeed[3] = UbxReceiver.buf[69];

    GpsMinimum.direction[0] = UbxReceiver.buf[70];
    GpsMinimum.direction[1] = UbxReceiver.buf[71];
    GpsMinimum.direction[2] = UbxReceiver.buf[72];
    GpsMinimum.direction[3] = UbxReceiver.buf[73];
}

void update_gps_fix_data (void) {
    get_pvt_str(); // UbxReceiver.buf[0 ... 100] is now populated with the ubx-nav-pvt reply.

    sort_pvt_data_into_gps_struct();

}

bool have_3d_fix(void) {
    uint8_t err_ctr = 0;
    while (!ubx_send_and_recv(ubx_poll_status, 1, 24)) {
        if (err_ctr > 2) hard_lockup(GPS_COMM_LOCK);
        err_ctr++;
        __delay_ms(100);
    }
    if (eval_ubx_buf_for_fix()) return true;

    return false;

}
