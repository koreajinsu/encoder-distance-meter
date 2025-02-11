#include "lcd.h"
#include "i2c.h"

#define LCD_ADDR 0x27

void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t data = value & 0xF0;
    data |= mode;
    data |= 0x08;

    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | 0x04);
    _delay_us(1);
    i2c_write(data & ~0x04);
    _delay_us(50);
    i2c_stop();
}

void lcd_command(uint8_t command) {
    lcd_send(command & 0xF0, 0x00);
    lcd_send((command << 4), 0x00);
}

void lcd_data(uint8_t data) {
    lcd_send(data & 0xF0, 0x01);
    lcd_send((data << 4), 0x01);
}

void lcd_init() {
    _delay_ms(50);
    lcd_command(0x30);
    _delay_ms(5);
    lcd_command(0x30);
    _delay_us(150);
    lcd_command(0x20);
    _delay_ms(5);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_clear() {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_write_string(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}
