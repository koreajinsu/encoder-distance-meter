#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>

void lcd_init();
void lcd_clear();
void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void lcd_write_string(const char *str);

#endif
