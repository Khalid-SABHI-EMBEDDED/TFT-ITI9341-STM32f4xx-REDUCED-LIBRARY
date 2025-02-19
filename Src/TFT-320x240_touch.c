#include "stm32f4xx_hal.h"
#include "TFT-320x240_touch.h"

#define TFT_TOUCH_READ_X 0xD0
#define TFT_TOUCH_READ_Y 0x90

bool TFT_TouchPressed() {
    return HAL_GPIO_ReadPin(TFT_TOUCH_IRQ_GPIO_PORT, TFT_TOUCH_IRQ_PIN) == GPIO_PIN_RESET;
}

bool TFT_TouchGetCoordinates(uint16_t* x, uint16_t* y) {
    static const uint8_t cmd_read_x[] = { TFT_TOUCH_READ_X };
    static const uint8_t cmd_read_y[] = { TFT_TOUCH_READ_Y };
    static const uint8_t zeroes_tx[] = { 0x00, 0x00 };

    // TFT Touch Select
    HAL_GPIO_WritePin(TFT_TOUCH_CS_GPIO_PORT, TFT_TOUCH_CS_PIN, GPIO_PIN_RESET);

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;

    for (uint8_t i = 0; i < 16; i++) {
        if (!TFT_TouchPressed())
            break;

        nsamples++;

        // Read Y coordinate
        HAL_SPI_Transmit(&TFT_TOUCH_SPI_PORT, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
        uint8_t y_raw[2];
        HAL_SPI_TransmitReceive(&TFT_TOUCH_SPI_PORT, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);

        // Read X coordinate
        HAL_SPI_Transmit(&TFT_TOUCH_SPI_PORT, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
        uint8_t x_raw[2];
        HAL_SPI_TransmitReceive(&TFT_TOUCH_SPI_PORT, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    // TFT Touch Unselect
    HAL_GPIO_WritePin(TFT_TOUCH_CS_GPIO_PORT, TFT_TOUCH_CS_PIN, GPIO_PIN_SET);

    if (nsamples < 16)
        return false;

    // Normalize raw X coordinate
    uint32_t raw_x = (avg_x / 16);
    if (raw_x < TFT_TOUCH_MIN_RAW_X) raw_x = TFT_TOUCH_MIN_RAW_X;
    if (raw_x > TFT_TOUCH_MAX_RAW_X) raw_x = TFT_TOUCH_MAX_RAW_X;

    // Normalize raw Y coordinate
    uint32_t raw_y = (avg_y / 16);
    if (raw_y < TFT_TOUCH_MIN_RAW_Y) raw_y = TFT_TOUCH_MIN_RAW_Y;
    if (raw_y > TFT_TOUCH_MAX_RAW_Y) raw_y = TFT_TOUCH_MAX_RAW_Y;

    // Uncomment this line to calibrate touchscreen:
    // UART_Printf("raw_x = %d, raw_y = %d\r\n", raw_x, raw_y);

    // Scale coordinates to screen dimensions
    *x = (raw_x - TFT_TOUCH_MIN_RAW_X) * TFT_TOUCH_SCALE_X / (TFT_TOUCH_MAX_RAW_X - TFT_TOUCH_MIN_RAW_X);
    *y = (raw_y - TFT_TOUCH_MIN_RAW_Y) * TFT_TOUCH_SCALE_Y / (TFT_TOUCH_MAX_RAW_Y - TFT_TOUCH_MIN_RAW_Y);

    return true;
}
