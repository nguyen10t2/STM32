#ifndef __7_LED_H__
#define __7_LED_H__

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEG_A_1 GPIO_PIN_0
#define SEG_B_1 GPIO_PIN_1
#define SEG_C_1 GPIO_PIN_7
#define SEG_D_1 GPIO_PIN_3
#define SEG_E_1 GPIO_PIN_4
#define SEG_F_1 GPIO_PIN_5
#define SEG_G_1 GPIO_PIN_6
#define SEG_PORT_1 GPIOC

// Mảng chứa các chân GPIO tương ứng với 8 LED
extern const uint16_t led_pins[];

// Số lượng LED (8)
extern const uint8_t led_count;

/**
 * @brief Tắt tất cả các LED.
 */
void led_all_off(void);

/**
 * @brief Bật tất cả các LED.
 */
void led_all_on(void);

/**
 * @brief Hiển thị một LED duy nhất dựa trên chỉ số.
 * @param index Chỉ số của LED (0-7)
 */
void led_show_single(uint8_t index);

/**
 * @brief Điều khiển 8 LED theo các hiệu ứng khác nhau dựa trên chế độ được chọn.
 * @param mode Chế độ hiệu ứng (1-4)
 * @param effect_delay_ms Thời gian trễ giữa các bước hiệu ứng (tính bằng ms)
 */
void display_splot(uint8_t mode, uint32_t effect_delay_ms);

/**
 * @brief Chạy 1 bước hiệu ứng (non-blocking), gọi lặp lại trong while(1).
 * @param mode Chế độ hiệu ứng (1-4)
 * @param effect_interval_ms Chu kỳ giữa các bước hiệu ứng (ms)
 */
void display_splot_step(uint8_t mode, uint32_t effect_interval_ms);

#ifdef __cplusplus
}
#endif

#endif /* __7_LED_H__ */