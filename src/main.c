/*
 * File:   main.c
 * Author: root
 *
 * Created on February 29, 2016, 10:01 AM
 */


#include <xc.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <io.h>
#include <eusart.h>
#include <ubxcmd.h>
#include <i2c.h>
#include <eeprom.h>
#include <error.h>
#include <am1805.h>
#include <brex.h>
#include <ichex.h>
#include <cti.h>

// CONFIG1
#pragma config FOSC = ECH       // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#pragma warning disable 752

#define _XTAL_FREQ 2000000

extern volatile UbxReceiverStruct UbxReceiver;

uint8_t tmr0_overflow = 0;
uint8_t data[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

void interrupt ISR() {
    if (TMR0IF == 1) {
        #asm
            incf     _tmr0_overflow,f
        #endasm
        TMR0IF = 0;
    }
    if (RCIF == 1) {
        UbxReceiver.buf[UbxReceiver.idx++] = RCREG;
        if (UbxReceiver.idx == 101) UbxReceiver.idx = 0;
        tmr0_overflow = 0;
    }
}

void main(void) {

    pic_init(); // Init the clock, io, i2c, uart, etc.

    if (is_this_the_first_pwrup()) {
        DEBUG("This is the first pwrup.\r\n");
        first_pwrup_calls();
    }
        // Is this the first time powering up the aqua?
        // If so, write defaults to eeprom, etc.

    if (is_usb_plugged_in()); //TODO - Implement 'usbloop()'

    PULL_ENSUREPSW_LOW();

    if (is_tc_high()) {
        DEBUG("TC is high.\r\n");
        sleep_start();
    }

    if (am1805_are_we_asleep()) {
        DEBUG("Sleep error.\r\n");
        am1805_sleep_error(); //TODO - Notes in function
    }
    // Here I don't think I need to reset the AM1805 sleep pararms. this
    // is because we should never be asleep when reaching this. An EXTI
    // interrupt must have preceeded this event.

    let_nrf_update_eeprom();

    reset_instruction_ptr();

    uint8_t mode = GET_INSTRUCTION_MODE();

    if (mode == CONT_INTERVAL_MODE) {
        DEBUG("Entering CTI mode.\r\n");
        cti_instruction_loop();
    } else if (mode == GMT_START_MODE) {
        hard_lockup(UNIMP_LOCK);
    } else {
        hard_lockup(HARD_LOCK);
    }

    while(1) {
        DEBUG("Reached End...????\r\n");
        blink();
        __delay_ms(1000);
    }

    return;
}
