#include "stm32f4xx_hal.h"
#include "TFT-320x240.h"

// Static function prototypes
static void TFT_WriteCommand(uint8_t cmd);
static void TFT_WriteData(uint8_t* buffer, size_t buffer_size);
static void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void TFT_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);

// Helper functions
static void TFT_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(TFT_DC_GPIO_PORT, TFT_DC_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&TFT_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

static void TFT_WriteData(uint8_t* buffer, size_t buffer_size) {
    HAL_GPIO_WritePin(TFT_DC_GPIO_PORT, TFT_DC_PIN, GPIO_PIN_SET);

    // Split data into small chunks because HAL can't send more than 64K at once
    while (buffer_size > 0) {
        uint16_t chunk_size = buffer_size > 32768 ? 32768 : buffer_size;
        HAL_SPI_Transmit(&TFT_SPI_PORT, buffer, chunk_size, HAL_MAX_DELAY);
        buffer += chunk_size;
        buffer_size -= chunk_size;
    }
}

static void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address set
    TFT_WriteCommand(0x2A); // CASET
    {
        uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
        TFT_WriteData(data, sizeof(data));
    }

    // Row address set
    TFT_WriteCommand(0x2B); // RASET
    {
        uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
        TFT_WriteData(data, sizeof(data));
    }

    // Write to RAM
    TFT_WriteCommand(0x2C); // RAMWR
}

// Initialization function
void TFT_Init() {
    // TFT Select
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

    // TFT Reset
    HAL_GPIO_WritePin(TFT_RESET_GPIO_PORT, TFT_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(TFT_RESET_GPIO_PORT, TFT_RESET_PIN, GPIO_PIN_SET);

    // Command list based on https://github.com/martnak/STM32-ILI9341

    // SOFTWARE RESET
    TFT_WriteCommand(0x01);
    HAL_Delay(1000);

    // POWER CONTROL A
    TFT_WriteCommand(0xCB);
    {
        uint8_t data[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
        TFT_WriteData(data, sizeof(data));
    }

    // POWER CONTROL B
    TFT_WriteCommand(0xCF);
    {
        uint8_t data[] = { 0x00, 0xC1, 0x30 };
        TFT_WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL A
    TFT_WriteCommand(0xE8);
    {
        uint8_t data[] = { 0x85, 0x00, 0x78 };
        TFT_WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL B
    TFT_WriteCommand(0xEA);
    {
        uint8_t data[] = { 0x00, 0x00 };
        TFT_WriteData(data, sizeof(data));
    }

    // POWER ON SEQUENCE CONTROL
    TFT_WriteCommand(0xED);
    {
        uint8_t data[] = { 0x64, 0x03, 0x12, 0x81 };
        TFT_WriteData(data, sizeof(data));
    }

    // PUMP RATIO CONTROL
    TFT_WriteCommand(0xF7);
    {
        uint8_t data[] = { 0x20 };
        TFT_WriteData(data, sizeof(data));
    }

    // POWER CONTROL, VRH[5:0]
    TFT_WriteCommand(0xC0);
    {
        uint8_t data[] = { 0x23 };
        TFT_WriteData(data, sizeof(data));
    }

    // POWER CONTROL, SAP[2:0]; BT[3:0]
    TFT_WriteCommand(0xC1);
    {
        uint8_t data[] = { 0x10 };
        TFT_WriteData(data, sizeof(data));
    }

    // VCM CONTROL
    TFT_WriteCommand(0xC5);
    {
        uint8_t data[] = { 0x3E, 0x28 };
        TFT_WriteData(data, sizeof(data));
    }

    // VCM CONTROL 2
    TFT_WriteCommand(0xC7);
    {
        uint8_t data[] = { 0x86 };
        TFT_WriteData(data, sizeof(data));
    }

    // MEMORY ACCESS CONTROL
    TFT_WriteCommand(0x36);
    {
        uint8_t data[] = { TFT_ROTATION };
        TFT_WriteData(data, sizeof(data));
    }

    // PIXEL FORMAT
    TFT_WriteCommand(0x3A);
    {
        uint8_t data[] = { 0x55 }; // 16-bit pixel format
        TFT_WriteData(data, sizeof(data));
    }

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    TFT_WriteCommand(0xB1);
    {
        uint8_t data[] = { 0x00, 0x18 };
        TFT_WriteData(data, sizeof(data));
    }

    // DISPLAY FUNCTION CONTROL
    TFT_WriteCommand(0xB6);
    {
        uint8_t data[] = { 0x08, 0x82, 0x27 };
        TFT_WriteData(data, sizeof(data));
    }

    // 3GAMMA FUNCTION DISABLE
    TFT_WriteCommand(0xF2);
    {
        uint8_t data[] = { 0x00 };
        TFT_WriteData(data, sizeof(data));
    }

    // GAMMA CURVE SELECTED
    TFT_WriteCommand(0x26);
    {
        uint8_t data[] = { 0x01 };
        TFT_WriteData(data, sizeof(data));
    }

    // POSITIVE GAMMA CORRECTION
    TFT_WriteCommand(0xE0);
    {
        uint8_t data[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                           0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
        TFT_WriteData(data, sizeof(data));
    }

    // NEGATIVE GAMMA CORRECTION
    TFT_WriteCommand(0xE1);
    {
        uint8_t data[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                           0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
        TFT_WriteData(data, sizeof(data));
    }

    // EXIT SLEEP MODE
    TFT_WriteCommand(0x11);
    HAL_Delay(120);

    // TURN ON DISPLAY
    TFT_WriteCommand(0x29);

    // TFT Unselect
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

// Drawing functions

static void TFT_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    TFT_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

    for (i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++) {
            if ((b << j) & 0x8000) {
                uint8_t data[] = { color >> 8, color & 0xFF };
                TFT_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                TFT_WriteData(data, sizeof(data));
            }
        }
    }
}

void TFT_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    // TFT Select
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

    while (*str) {
        if (x + font.width >= TFT_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height >= TFT_HEIGHT) break;
        }

        TFT_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    // TFT Unselect
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

void TFT_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    if (x + w > TFT_WIDTH) w = TFT_WIDTH - x;
    if (y + h > TFT_HEIGHT) h = TFT_HEIGHT - y;

    // TFT Select
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_RESET);

    TFT_SetAddressWindow(x, y, x + w - 1, y + h - 1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(TFT_DC_GPIO_PORT, TFT_DC_PIN, GPIO_PIN_SET);
    for (uint32_t i = 0; i < w * h; i++) {
        HAL_SPI_Transmit(&TFT_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
    }

    // TFT Unselect
    HAL_GPIO_WritePin(TFT_CS_GPIO_PORT, TFT_CS_PIN, GPIO_PIN_SET);
}

void TFT_FillScreen(uint16_t color) {
    TFT_FillRectangle(0, 0, TFT_WIDTH, TFT_HEIGHT, color);
}




