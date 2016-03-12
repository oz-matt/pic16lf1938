#ifndef AM1805_H
#define AM1805_H

#include    <stdint.h>
#include    <stdbool.h>

#define     AM1805_WRITE_ADDRESS        0xD2
#define     AM1805_READ_ADDRESS         0xD3

// ---------------------- TIME TO PROGRAM INTO AM1805 ------------------------- //
#define     PGM_TIME_SEC                0           // xTTTOOOO  T = Tens place
#define     PGM_TIME_MIN                0b00010101  // xTTTOOOO
#define     PGM_TIME_HR                 0b00001001  // xxTTOOOO
#define     PGM_TIME_DAY                0b00000110  // xxTTOOOO
#define     PGM_TIME_MO                 0b00000011  // xxxTOOOO
#define     PGM_TIME_YR                 0b00010110  // TTTTOOOO
// ---------------------------------------------------------------------------- //

// ------------------- ALARM TIME TO PROGRAM INTO AM1805 ---------------------- //
#define     PGM_ALARM_TIME_SEC          0           // xTTTOOOO  T = Tens place
#define     PGM_ALARM_TIME_MIN          0b00010101  // xTTTOOOO
#define     PGM_ALARM_TIME_HR           0b00001001  // xxTTOOOO
#define     PGM_ALARM_TIME_DAY          0b00000111  // xxTTOOOO
#define     PGM_ALARM_TIME_MO           0b00000011  // xxxTOOOO
#define     PGM_ALARM_TIME_YR           0b00010110  // TTTTOOOO
// ---------------------------------------------------------------------------- //

typedef struct {
    uint8_t     sec;
    uint8_t     min;
    uint8_t     hr;
    uint8_t     day;
    uint8_t     mo;
    uint8_t     yr;
} PicTimeStruct;

PicTimeStruct PicTime;

void am1805_write_defaults(void);
void am1805_start_ctdn_tmr (uint8_t amt_time);
void am1805_initiate_ctdn_sleep (void);
void am1805_print_register(uint8_t reg);
bool am1805_are_we_asleep(void);
void am1805_sleep_error(void);
void am1805_update_pic_time(void);
void am1805_write_pgm_time(void);
void am1805_write_alarm_pgm_time(void);

#endif  /* AM1805 */
