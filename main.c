#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "lcd.h"
#include "interrupt.h"
#include "button.h"

#define BUTTON_C PD4

int main() {
    i2c_init();
    lcd_init();
    interrupts_init();

    DDRD &= ~(1 << BUTTON_C);
    PORTD |= (1 << BUTTON_C);

    lcd_clear();
    lcd_write_string("Hello, World!");

    while (1) {
        check_button_c();
    }
}
