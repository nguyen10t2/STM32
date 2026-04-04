#include "led_7.h"

const uint8_t digit_map[16] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // B
    0b00111001, // C
    0b01011110, // D
    0b01111001, // E
    0b01110001  // F
};

void set_segments(uint8_t num) {
    if (num > 15) {
        return;
    }

    uint8_t val = digit_map[num];

    HAL_GPIO_WritePin(SEG_PORT, SEG_A, (val >> 0) & 1);
    HAL_GPIO_WritePin(SEG_PORT, SEG_B, (val >> 1) & 1);
    HAL_GPIO_WritePin(SEG_PORT, SEG_C, (val >> 2) & 1);
    HAL_GPIO_WritePin(SEG_PORT, SEG_D, (val >> 3) & 1);
    HAL_GPIO_WritePin(SEG_PORT, SEG_E, (val >> 4) & 1);
    HAL_GPIO_WritePin(SEG_PORT, SEG_F, (val >> 5) & 1);
    HAL_GPIO_WritePin(SEG_PORT, SEG_G, (val >> 6) & 1);
}

void display_2digit(uint8_t d1, uint8_t d2, uint32_t DELAY_MS) {
    set_segments(d1);
    HAL_GPIO_WritePin(DIG_PORT, DIG1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DIG_PORT, DIG2, GPIO_PIN_RESET);
    HAL_Delay(DELAY_MS);

    set_segments(d2);
    HAL_GPIO_WritePin(DIG_PORT, DIG1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIG_PORT, DIG2, GPIO_PIN_SET);
    HAL_Delay(DELAY_MS);
}

void display_hex_byte(uint8_t byte, uint32_t DELAY_MS) {
    uint8_t high = (byte >> 4) & 0x0F;  // High nibble (bit 7-4)
    uint8_t low = byte & 0x0F;          // Low nibble (bit 3-0)
    display_2digit(high, low, DELAY_MS);
}