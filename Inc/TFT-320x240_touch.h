#ifndef __TFT_TOUCH_H__
#define __TFT_TOUCH_H__

#include <stdbool.h>

/*** Redefine if necessary ***/

// Warning! Use SPI bus with < 1.3 Mbit speed, better ~650 Kbit to be safe.
#define TFT_TOUCH_SPI_PORT hspi1
extern SPI_HandleTypeDef TFT_TOUCH_SPI_PORT;

#define TFT_TOUCH_IRQ_PIN        GPIO_PIN_4 // Arduino D5
#define TFT_TOUCH_IRQ_GPIO_PORT  GPIOB
#define TFT_TOUCH_CS_PIN         GPIO_PIN_10 // Arduino D2
#define TFT_TOUCH_CS_GPIO_PORT   GPIOA

// Change depending on screen orientation
#define TFT_TOUCH_SCALE_X 240
#define TFT_TOUCH_SCALE_Y 320

// To calibrate, uncomment UART_Printf line in TFT-320x240_touch.c
#define TFT_TOUCH_MIN_RAW_X 1500
#define TFT_TOUCH_MAX_RAW_X 31000
#define TFT_TOUCH_MIN_RAW_Y 3276
#define TFT_TOUCH_MAX_RAW_Y 30110

bool TFT_TouchGetCoordinates(uint16_t* x, uint16_t* y);

#endif // __TFT_TOUCH_H__
