#include "button.h"
#include "lcd.h"
#include <util/delay.h>

#define BUTTON_C PD4

extern volatile uint8_t lastButtonState;

void check_button_c() {
    if (!(PIND & (1 << BUTTON_C))) {
        _delay_ms(50);
        if (!(PIND & (1 << BUTTON_C))) {
            if (lastButtonState != 3) {
                lastButtonState = 3;
                lcd_clear();
                lcd_write_string("C");
            } else {
                lastButtonState = 0;
                lcd_clear();
                lcd_write_string("Hello, World!");
            }
            while (!(PIND & (1 << BUTTON_C)));
        }
    }
}
