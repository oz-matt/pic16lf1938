#include <stdint.h>
#include <stdbool.h>

#ifndef UBXCMD_H
#define UBXCMD_H

#define     BEGIN_UART_LISTEN   RCIE = 1; \ // Enable UART receive interrupt
                                __delay_ms(1); \
                                reset_ubx_struct();

#define     END_UART_LISTEN     tmr0_overflow=0; \
                                RCIE = 0; // Disable UART receive interrupt


/* ------------------------------------------------- //
!! - MUST PRECEED ALL POLL STRINGS WITH THE LENGTH - !!
// ------------------------------------------------- */

        // LENGTH ------------v
uint8_t ubx_poll_status[] = {8, 0xB5, 0x62, 0x01, 0x03,
        0x00, 0x00, 0x04, 0x0D};

uint8_t ubx_poll_posllr[] = {8, 0xB5, 0x62, 0x01, 0x02,
        0x00, 0x00, 0x03, 0x0A};

uint8_t ubx_poll_pvt[] = {8, 0xB5, 0x62, 0x01, 0x07, 0x00,
        0x00, 0x08, 0x19};

uint8_t ubx_disable_gxgga[] = {11, 0xB5, 0x62, 0x06, 0x01,
        0x03, 0x00, 0xF0, 0x00, 0x00, 0xFA, 0x0F};

uint8_t ubx_disable_gxgll[] = {11, 0xB5, 0x62, 0x06, 0x01,
        0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};

uint8_t ubx_disable_gxgsa[] = {11, 0xB5, 0x62, 0x06, 0x01,
        0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};

uint8_t ubx_disable_gxgsv[] = {11, 0xB5, 0x62, 0x06, 0x01,
        0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};

uint8_t ubx_disable_gxrmc[] = {11, 0xB5, 0x62, 0x06, 0x01,
        0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17};

uint8_t ubx_disable_gxvtg[] = {11, 0xB5, 0x62, 0x06, 0x01,
        0x03, 0x00, 0xF0, 0x05, 0x00, 0xFF, 0x19};

uint8_t ubx_nmea_disable_ack[] = {0xB5, 0x62, 0x05, 0x01,
    0x02, 0x00, 0x06, 0x01, 0x0F, 0x38};

typedef struct { // All these values are stored as LITTLE-ENDIAN
    uint8_t year[2];
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t valid_flags;    //Bitfield 'valid' in UBX-NAV-PVT
    uint8_t gps_flags;      //Bitfield 'flags' in UBX-NAV-PVT
    uint8_t time_flags;     //Bitfield 'flags2' in UBX-NAV-PVT

    uint8_t num_sat;

    uint8_t lon[4];
    uint8_t lat[4];

    uint8_t height[4];

    uint8_t gspeed[4];

    uint8_t direction[4];
} GpsMinimumStruct;

typedef struct {
    uint8_t buf[102];  // These get initialized before you use them, everytime
    uint8_t idx;
} UbxReceiverStruct;

void reset_ubx_struct(void);
void ubx_send_str(char *str);
bool ubx_send_and_recv(char *ubx_cmd, uint8_t response_time, uint8_t expected_len);
void ubx_silence_nmea (void);
void update_gps_fix_data (void);
bool have_3d_fix(void);

#endif  /* UBXCMD_H */
