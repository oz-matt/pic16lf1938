
#ifndef io_h
#define io_h

#include <stdint.h>
#include <stdbool.h>

#define RED     0
#define GREEN   1
#define BLUE    2

#define DEBUG(X)            uart_write_string(X)

#define MPGATE              0x01
#define NRFCOMPLETELV       0x02
#define VCOMP               0x04
#define NRF_EN              0x08
#define GPS_EN              0x10
#define EEPROM_MASTER_SEL   0x20
#define N_CHRG              0x40
#define PSWSW               0x80

#define LED_BLU             0x01
#define EHS6_EN             0x02
#define NRF_USING_EEPROMLV  0x04
#define ENSUREPSW           0x08
#define TC                  0x10
#define MSWITCH             0x20

#define USBON               0x01
#define LED_GRN             0x02
#define LED_RED             0x04
#define PIC_SDA             0x08
#define PIC_SCL             0x10
#define BFAULTLV            0x20
#define GPS_RXD_LV          0x40
#define GPS_TXD_LV          0x80

#define TMR0_START_ISR      TMR0 = 0; \
                            TMR0IE = 1; \
                            tmr0_overflow = 0;

#define TMR0_STOP           TMR0IE = 0;

#define PULL_ENSUREPSW_LOW()                LATB &= ~(ENSUREPSW)
#define PULL_ENSUREPSW_HIGH()               LATB |= ENSUREPSW

#define PULL_EEPROM_MASTER_SEL_HIGH()       LATB |= EEPROM_MASTER_SEL
#define PULL_EEPROM_MASTER_SEL_LOW()        LATB &= ~(EEPROM_MASTER_SEL)
#define ENABLE_NRF52()                      LATA |= NRF_EN; \
                                            PULL_EEPROM_MASTER_SEL_HIGH()
#define DISABLE_NRF52()                     LATA &= (NRF_EN); \
                                            PULL_EEPROM_MASTER_SEL_LOW()
#define ENABLE_GPS()                        LATA |= GPS_EN
#define DISABLE_GPS()                       LATA &= ~(GPS_EN)
#define ENABLE_EHS6()                       LATB |= EHS6_EN
#define DISABLE_EHS6()                      LATB &= ~(EHS6_EN)

#define CONT_INTERVAL_MODE  1
#define GMT_START_MODE      2

void set_led_duty(uint8_t color, uint16_t duty);
bool is_usb_plugged_in (void);
bool is_tc_high  (void);
void blink (void);
bool is_nrfcomplete_high (void);
void pic_init(void);

#endif  /*io_h*/
