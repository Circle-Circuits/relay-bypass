/*
 * File:   main.c
 * Author: Circle Circuits
 *
 * Created on September 18, 2021, 9:48 PM
 */

// Supplementary libraries to use variables and enable XC8 compatibility 
#include <stdint.h>
#include <xc.h>
// Include configuration bits
#include "header.h"

int switchState(int state, int relayType) { 
    if (relayType == 0) { // momentary relay, send continuous 5v
       if (state == 0) {
        GP2 = 1;
        GP0 = 1;
        return 1;
        } 
    else if (state == 1) {
            state = 0;
            GP2 = 0;
            GP0 = 0;
            return 0;
        } 
    }
    else if (relayType == 1) { // latching relay, send a 5v pulse
        if (state == 0) {
            GP2 = 0;
            GP5 = 0;
            GP0 = 1;
            __delay_ms(20);
            GP0 = 0;
            return 1;
        } 
        else if (state == 1) {
            state = 0;
            GP2 = 1;
            GP0 = 0;
            GP5 = 1;
            __delay_ms(20);
            GP5 = 0;
            return 0;
        } 
    }
}

void blink(int n) {
    for (int i=0; i<n; i=i+1) {
        GP2 = 0;
        __delay_ms(100);
        GP2 = 1;
        __delay_ms(100); 
        GP2 = 0;
    }
}

int toggleMomentary(int momentary) {
    if (momentary == 0) momentary = 1;
    else momentary = 0;
    eeprom_write(0, momentary);
    blink(3);
    __delay_ms(500);
    return momentary;
}

void main(void) {
    int state; // pedal is on or off
    int pressed; // incremented when footswitch remains pressed
    int momentary; // momentary trigger on / off
    int relayType; // 0 is single side stable; 1 is single coil latching
    
    state = 0;
    pressed = 0;
    momentary = (int)eeprom_read(0);
    relayType = 1;
    
    ANSEL = 0; // no analog GPIO
    CMCON = 0x07; // comparator off 
    ADCON0 = 0; // ADC and DAC converters off 
    OPTION_REG = 0b01010001; // GPIO pull-ups are enabled
    WPU = 0b0010010; // enable pull up on GPIO 01 and 04
    
    TRISIO0 = 0; // pin 7 for Relay+
    TRISIO1 = 1; // pin 6 for Switch 
    TRISIO2 = 0; // pin 5 for LED 
    TRISIO3 = 0; // pin 4 is unused
    TRISIO4 = 1; // pin 3 is unused
    TRISIO5 = 0; // pin 2 is for Relay-
    
    GPIO = 0; // all the GPIOs are in low state (0V) when starting
    
    // Hold switch down on startup to deactivate momentary switch feature. LED will blink.
    if (GP1 == 0) momentary = toggleMomentary(momentary);
    
    while(1) {
        if (GP1 == 0) {
            if (pressed == 0) {
                pressed = 1;
                state = switchState(state, relayType);
            } 
            else if (pressed < 100 && momentary == 1) {
                ++pressed;
            }
        } else if (GP1 == 1) {
            if (pressed > 35) { // if it was momentary on, turn off
                state = switchState(state, relayType);
            }
            pressed = 0;
        }
        __delay_ms(15); // debounce
    }
}