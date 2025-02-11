#include "interrupt.h"
#include "lcd.h"

volatile uint8_t lastButtonState = 0;

void interrupts_init() {
    EICRA |= (1 << ISC01) | (1 << ISC00) | (1 << ISC11) | (1 << ISC10);
    EIMSK |= (1 << INT0) | (1 << INT1);
    sei();
}

ISR(INT0_vect) {
    if (lastButtonState != 1) {
        lastButtonState = 1;
        lcd_clear();
        lcd_write_string("A");
    } else {
        lastButtonState = 0;
        lcd_clear();
        lcd_write_string("Hello, World!");
    }
}

ISR(INT1_vect) {
    if (lastButtonState != 2) {
        lastButtonState = 2;
        lcd_clear();
        lcd_write_string("B");
    } else {
        lastButtonState = 0;
        lcd_clear();
        lcd_write_string("Hello, World!");
    }
}
