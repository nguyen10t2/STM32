#include "7_led.h"

const uint16_t led_pins[] = {
	SEG_A_1,
	SEG_B_1,
	SEG_C_1,
	SEG_D_1,
	SEG_E_1,
	SEG_F_1,
	SEG_G_1,
};

const uint8_t led_count = (uint8_t)(sizeof(led_pins) / sizeof(led_pins[0]));

void led_all_off(void) {
	for (uint8_t i = 0; i < led_count; i++) {
		HAL_GPIO_WritePin(SEG_PORT_1, led_pins[i], GPIO_PIN_RESET);
	}
}

void led_all_on(void) {
	for (uint8_t i = 0; i < led_count; i++) {
		HAL_GPIO_WritePin(SEG_PORT_1, led_pins[i], GPIO_PIN_SET);
	}
}

void led_show_single(uint8_t index) {
	led_all_off();
	HAL_GPIO_WritePin(SEG_PORT_1, led_pins[index], GPIO_PIN_SET);
}

void display_splot(uint8_t mode, uint32_t effect_delay_ms) {
	switch (mode) {
		case 1:
			for (uint8_t i = 0; i < led_count; i++) {
				led_show_single(i);
				HAL_Delay(effect_delay_ms);
			}
			led_all_off();
			break;

		case 2:
			for (int8_t i = (int8_t)led_count - 1; i >= 0; i--) {
				led_show_single((uint8_t)i);
				HAL_Delay(effect_delay_ms);
			}
			led_all_off();
			break;

		case 3:
			led_all_on();
			HAL_Delay(effect_delay_ms);
			led_all_off();
			HAL_Delay(effect_delay_ms);
			break;

		case 4: {
			uint8_t left = 0;
			uint8_t right = led_count - 1;

			while (left < right) {
				led_all_off();
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[left], GPIO_PIN_SET);
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[right], GPIO_PIN_SET);
				HAL_Delay(effect_delay_ms);
				left++;
				right--;
			}

			if (left == right) {
				led_show_single(left);
				HAL_Delay(effect_delay_ms);
			} else {
				led_all_off();
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[right + 1], GPIO_PIN_SET);
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[left - 1], GPIO_PIN_SET);
				HAL_Delay(effect_delay_ms);
			}

			while (left > 0 && right < (led_count - 1)) {
				left--;
				right++;
				led_all_off();
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[left], GPIO_PIN_SET);
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[right], GPIO_PIN_SET);
				HAL_Delay(effect_delay_ms);
			}

			led_all_off();
			break;
		}

		default:
			led_all_off();
			break;
	}
}

void display_splot_step(uint8_t mode, uint32_t effect_interval_ms) {
	static uint32_t last_tick = 0;
	static uint8_t prev_mode = 0;
	static uint8_t idx = 0;
	static int8_t dir = 1;

	uint32_t now = HAL_GetTick();

	if (mode < 1 || mode > 4) {
		led_all_off();
		return;
	}

	if (mode != prev_mode) {
		prev_mode = mode;
		idx = 0;
		dir = 1;
		led_all_off();
	}

	if ((now - last_tick) < effect_interval_ms) {
		return;
	}
	last_tick = now;

	switch (mode) {
		case 1:
			led_show_single(idx);
			idx = (uint8_t)((idx + 1U) % led_count);
			break;

		case 2:
			led_show_single((uint8_t)(led_count - 1U - idx));
			idx = (uint8_t)((idx + 1U) % led_count);
			break;

		case 3:
			if (idx == 0U) {
				led_all_on();
				idx = 1U;
			} else {
				led_all_off();
				idx = 0U;
			}
			break;

		case 4:
			led_all_off();
			if (dir > 0) {
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[idx], GPIO_PIN_SET);
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[led_count - 1U - idx], GPIO_PIN_SET);
				if (idx >= (led_count / 2U)) {
					dir = -1;
				} else {
					idx++;
				}
			} else {
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[idx], GPIO_PIN_SET);
				HAL_GPIO_WritePin(SEG_PORT_1, led_pins[led_count - 1U - idx], GPIO_PIN_SET);
				if (idx == 0U) {
					dir = 1;
				} else {
					idx--;
				}
			}
			break;

		default:
			led_all_off();
			break;
	}
}