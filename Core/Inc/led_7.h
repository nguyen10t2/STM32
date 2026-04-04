#ifndef __LED_7_H__
#define __LED_7_H__

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SEG_A GPIO_PIN_4
#define SEG_B GPIO_PIN_3
#define SEG_C GPIO_PIN_2
#define SEG_D GPIO_PIN_1
#define SEG_E GPIO_PIN_0
#define SEG_F GPIO_PIN_5
#define SEG_G GPIO_PIN_6
#define SEG_PORT GPIOB

#define DIG1 GPIO_PIN_7
#define DIG2 GPIO_PIN_8
#define DIG_PORT GPIOB

// Thời gian trễ giữa các bước hiển thị (tính bằng ms)
extern const uint8_t digit_map[];

/**
 * @brief Thiết lập các chân GPIO để hiển thị số trên 7-segment.
 * @param num Số cần hiển thị (0-F, hexa)
 */
void set_segments(uint8_t num);

/**
 * @brief Hiển thị hai chữ số trên 7-segment bằng cách sử dụng kỹ thuật quét.
 * @param d1 Chữ số hàng chục (0-9 hoặc 0-F nếu hexa)
 * @param d2 Chữ số hàng đơn vị (0-9 hoặc 0-F nếu hexa)
 * @param DELAY_MS Thời gian trễ giữa các bước hiển thị (tính bằng ms)
 */
void display_2digit(uint8_t d1, uint8_t d2, uint32_t DELAY_MS);

/**
 * @brief Hiển thị 1 byte dưới dạng 2 chữ số hex trên 7-segment.
 * @param byte Byte cần hiển thị (0x00 - 0xFF)
 * @param DELAY_MS Thời gian trễ giữa các bước hiển thị (tính bằng ms)
 */
void display_hex_byte(uint8_t byte, uint32_t DELAY_MS);

#ifdef __cplusplus
}
#endif

#endif