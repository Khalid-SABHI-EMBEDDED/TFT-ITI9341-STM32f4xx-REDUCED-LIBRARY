#include "fonts.h"
#include <stdbool.h>

// Display orientation
#define TFT_MADCTL_MY  0x80
#define TFT_MADCTL_MX  0x40
#define TFT_MADCTL_MV  0x20
#define TFT_MADCTL_ML  0x10
#define TFT_MADCTL_RGB 0x00
#define TFT_MADCTL_BGR 0x08
#define TFT_MADCTL_MH  0x04

// Default orientation
#define TFT_WIDTH  320
#define TFT_HEIGHT 240
#define TFT_ROTATION (TFT_MADCTL_MX | TFT_MADCTL_MY | TFT_MADCTL_MV | TFT_MADCTL_BGR)

// SPI and GPIO configuration
#define TFT_SPI_PORT hspi1
extern SPI_HandleTypeDef TFT_SPI_PORT;

#define TFT_RESET_PIN       GPIO_PIN_7
#define TFT_RESET_GPIO_PORT GPIOB
#define TFT_CS_PIN          GPIO_PIN_6
#define TFT_CS_GPIO_PORT    GPIOB
#define TFT_DC_PIN          GPIO_PIN_9
#define TFT_DC_GPIO_PORT    GPIOA

// Color definitions
#define TFT_BLACK   0x0000
#define TFT_BLUE    0x001F
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_CYAN    0x07FF
#define TFT_MAGENTA 0xF81F
#define TFT_YELLOW  0xFFE0
#define TFT_WHITE   0xFFFF
#define TFT_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

// Function prototypes
void TFT_Init(void);
void TFT_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void TFT_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void TFT_FillScreen(uint16_t color);


