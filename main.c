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

int switchState(int state) { 
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

void main(void) {
    uint8_t state; // pedal is on or off
    uint8_t pressed; // incremented when footswitch remains pressed
    
    state = 0;
    pressed = 0;
    
    ANSEL = 0; // no analog GPIO
    CMCON = 0x07; // comparator off 
    ADCON0 = 0; // ADC and DAC converters off 
    OPTION_REG = 0b01010001; // GPIO pull-ups are enabled
    WPU = 0b0010010; // enable pull up on GPIO 01 and 04
    
    TRISIO0 = 0; // pin 7 for LED
    TRISIO1 = 1; // pin 6 for switch sensor
    TRISIO2 = 0; // pin 5 for Relay trigger 
    TRISIO3 = 0; // pin 4 is unused
    TRISIO4 = 1; // pin 3 for Momentary on/off 
    TRISIO5 = 0; // pin 2 is unused
    
    GPIO = 0; // all the GPIOs are in low state (0V) when starting
    
    while(1) {
        if (GP1 == 0) {
            if (pressed == 0) {
                pressed = 1;
                state = switchState(state);
            } 
            else if (pressed < 100 && GP4 == 0) {
                ++pressed;
            }
        } else if (GP1 == 1) {
            if (pressed > 35) { // if it was momentary on, turn off
                state = switchState(state);
            }
            pressed = 0;
        }
        __delay_ms(15); // debounce
    }
}