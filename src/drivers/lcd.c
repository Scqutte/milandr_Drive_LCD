#include "drivers/lcd.h"

#include "config.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define LCD_DATA_PORT MDR_PORTA
#define LCD_CTRL_PORT MDR_PORTF

#define LCD_DATA_PINS (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | \
                       PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7)

#define LCD_E1_PIN PORT_Pin_0
#define LCD_E2_PIN PORT_Pin_1
#define LCD_RES_PIN PORT_Pin_2
#define LCD_RW_PIN PORT_Pin_3
#define LCD_A0_PIN PORT_Pin_4

#define LCD_CMD_DISPLAY_ON 0x3FU
#define LCD_CMD_SET_Y 0x40U
#define LCD_CMD_SET_PAGE 0xB8U
#define LCD_CMD_START_LINE 0xC0U

#define LCD_CHIP_WIDTH 64U
#define LCD_PAGE_HEIGHT 8U
#define LCD_PAGES (SCREEN_HEIGHT / LCD_PAGE_HEIGHT)

static uint8_t framebuffer[LCD_PAGES][SCREEN_WIDTH];

static void lcd_delay(void)
{
    volatile uint16_t i;

    for (i = 0; i < 80; i++) {
    }
}

static void lcd_init_port(MDR_PORT_TypeDef *port, uint16_t pins)
{
    PORT_InitTypeDef init;

    init.PORT_Pin = pins;
    init.PORT_OE = PORT_OE_OUT;
    init.PORT_PULL_UP = PORT_PULL_UP_OFF;
    init.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    init.PORT_PD_SHM = PORT_PD_SHM_OFF;
    init.PORT_PD = PORT_PD_DRIVER;
    init.PORT_GFEN = PORT_GFEN_OFF;
    init.PORT_FUNC = PORT_FUNC_PORT;
    init.PORT_SPEED = PORT_SPEED_FAST;
    init.PORT_MODE = PORT_MODE_DIGITAL;

    PORT_Init(port, &init);
}

static void lcd_write_bus(uint8_t value)
{
    PORT_Write(LCD_DATA_PORT, value);
}

static void lcd_set_data_mode(uint8_t is_data)
{
    if (is_data) {
        PORT_SetBits(LCD_CTRL_PORT, LCD_A0_PIN);
    } else {
        PORT_ResetBits(LCD_CTRL_PORT, LCD_A0_PIN);
    }
}

static void lcd_pulse(uint8_t chip)
{
    uint32_t enable_pin = (chip == 0U) ? LCD_E1_PIN : LCD_E2_PIN;

    PORT_SetBits(LCD_CTRL_PORT, enable_pin);
    lcd_delay();
    PORT_ResetBits(LCD_CTRL_PORT, enable_pin);
    lcd_delay();
}

static void lcd_write_raw(uint8_t chip, uint8_t value, uint8_t is_data)
{
    PORT_ResetBits(LCD_CTRL_PORT, LCD_RW_PIN);
    lcd_set_data_mode(is_data);
    lcd_write_bus(value);
    lcd_pulse(chip);
}

static void lcd_write_command(uint8_t chip, uint8_t command)
{
    lcd_write_raw(chip, command, 0);
}

static void lcd_write_data(uint8_t chip, uint8_t data)
{
    lcd_write_raw(chip, data, 1);
}

static const uint8_t *lcd_font_columns(char ch)
{
    static const uint8_t space[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    static const uint8_t colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const uint8_t digits[10][5] = {
        {0x3E, 0x51, 0x49, 0x45, 0x3E},
        {0x00, 0x42, 0x7F, 0x40, 0x00},
        {0x42, 0x61, 0x51, 0x49, 0x46},
        {0x21, 0x41, 0x45, 0x4B, 0x31},
        {0x18, 0x14, 0x12, 0x7F, 0x10},
        {0x27, 0x45, 0x45, 0x45, 0x39},
        {0x3C, 0x4A, 0x49, 0x49, 0x30},
        {0x01, 0x71, 0x09, 0x05, 0x03},
        {0x36, 0x49, 0x49, 0x49, 0x36},
        {0x06, 0x49, 0x49, 0x29, 0x1E},
    };
    static const uint8_t letters[26][5] = {
        {0x7E, 0x11, 0x11, 0x11, 0x7E},
        {0x7F, 0x49, 0x49, 0x49, 0x36},
        {0x3E, 0x41, 0x41, 0x41, 0x22},
        {0x7F, 0x41, 0x41, 0x22, 0x1C},
        {0x7F, 0x49, 0x49, 0x49, 0x41},
        {0x7F, 0x09, 0x09, 0x09, 0x01},
        {0x3E, 0x41, 0x49, 0x49, 0x7A},
        {0x7F, 0x08, 0x08, 0x08, 0x7F},
        {0x00, 0x41, 0x7F, 0x41, 0x00},
        {0x20, 0x40, 0x41, 0x3F, 0x01},
        {0x7F, 0x08, 0x14, 0x22, 0x41},
        {0x7F, 0x40, 0x40, 0x40, 0x40},
        {0x7F, 0x02, 0x0C, 0x02, 0x7F},
        {0x7F, 0x04, 0x08, 0x10, 0x7F},
        {0x3E, 0x41, 0x41, 0x41, 0x3E},
        {0x7F, 0x09, 0x09, 0x09, 0x06},
        {0x3E, 0x41, 0x51, 0x21, 0x5E},
        {0x7F, 0x09, 0x19, 0x29, 0x46},
        {0x46, 0x49, 0x49, 0x49, 0x31},
        {0x01, 0x01, 0x7F, 0x01, 0x01},
        {0x3F, 0x40, 0x40, 0x40, 0x3F},
        {0x1F, 0x20, 0x40, 0x20, 0x1F},
        {0x7F, 0x20, 0x18, 0x20, 0x7F},
        {0x63, 0x14, 0x08, 0x14, 0x63},
        {0x07, 0x08, 0x70, 0x08, 0x07},
        {0x61, 0x51, 0x49, 0x45, 0x43},
    };

    if ((ch >= 'a') && (ch <= 'z')) {
        ch = (char)(ch - 'a' + 'A');
    }

    if ((ch >= '0') && (ch <= '9')) {
        return digits[ch - '0'];
    }

    if ((ch >= 'A') && (ch <= 'Z')) {
        return letters[ch - 'A'];
    }

    if (ch == ':') {
        return colon;
    }

    return space;
}

void lcd_init(void)
{
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTF, ENABLE);

    lcd_init_port(LCD_DATA_PORT, LCD_DATA_PINS);
    lcd_init_port(LCD_CTRL_PORT,
                  LCD_E1_PIN | LCD_E2_PIN | LCD_RES_PIN | LCD_RW_PIN | LCD_A0_PIN);

    PORT_ResetBits(LCD_CTRL_PORT, LCD_E1_PIN | LCD_E2_PIN | LCD_RW_PIN | LCD_A0_PIN);

    PORT_ResetBits(LCD_CTRL_PORT, LCD_RES_PIN);
    lcd_delay();
    PORT_SetBits(LCD_CTRL_PORT, LCD_RES_PIN);
    lcd_delay();

    lcd_write_command(0, LCD_CMD_DISPLAY_ON);
    lcd_write_command(1, LCD_CMD_DISPLAY_ON);
    lcd_write_command(0, LCD_CMD_START_LINE);
    lcd_write_command(1, LCD_CMD_START_LINE);

    lcd_clear();
    lcd_flush();
}

void lcd_clear(void)
{
    uint8_t page;
    uint8_t x;

    for (page = 0; page < LCD_PAGES; page++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            framebuffer[page][x] = 0;
        }
    }
}

void lcd_draw_text(uint8_t x, uint8_t y, const char *text)
{
    while (*text != '\0') {
        uint8_t column;
        const uint8_t *glyph = lcd_font_columns(*text);

        for (column = 0; column < 5; column++) {
            uint8_t row;

            for (row = 0; row < 7; row++) {
                if ((glyph[column] & (1U << row)) != 0U) {
                    lcd_draw_pixel((uint8_t)(x + column), (uint8_t)(y + row), 1);
                }
            }
        }

        x = (uint8_t)(x + 6U);
        text++;
    }
}

void lcd_draw_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t page;
    uint8_t bit;

    if ((x >= SCREEN_WIDTH) || (y >= SCREEN_HEIGHT)) {
        return;
    }

    page = (uint8_t)(y / LCD_PAGE_HEIGHT);
    bit = (uint8_t)(1U << (y % LCD_PAGE_HEIGHT));

    if (color) {
        framebuffer[page][x] |= bit;
    } else {
        framebuffer[page][x] &= (uint8_t)~bit;
    }
}

void lcd_flush(void)
{
    uint8_t chip;
    uint8_t page;
    uint8_t x;

    for (chip = 0; chip < 2U; chip++) {
        for (page = 0; page < LCD_PAGES; page++) {
            lcd_write_command(chip, (uint8_t)(LCD_CMD_SET_PAGE | page));
            lcd_write_command(chip, LCD_CMD_SET_Y);

            for (x = 0; x < LCD_CHIP_WIDTH; x++) {
                lcd_write_data(chip, framebuffer[page][x + (chip * LCD_CHIP_WIDTH)]);
            }
        }
    }
}
