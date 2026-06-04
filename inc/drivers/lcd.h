#ifndef DRIVERS_LCD_H
#define DRIVERS_LCD_H

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_draw_text(uint8_t x, uint8_t y, const char *text);
void lcd_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void lcd_flush(void);
void lcd_fill_raw(uint8_t value);
uint8_t lcd_map_count(void);
void lcd_select_map(uint8_t map);

#endif
