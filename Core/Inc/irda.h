/*
 * irda.h
 *
 *  Created on: Nov 1, 2024
 *      Author: trung
 */

#ifndef SRC_IRDA_H_
#define SRC_IRDA_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/**
 * @brief Mảng chứa 4 byte của lệnh IR đã giải mã.
 * Cấu trúc cụ thể phụ thuộc vào loại remote và giao thức sử dụng.
 */
extern unsigned char irda_cmd[4];

/**
 * @brief Cờ được đặt thành 1 khi một lệnh IR hoàn chỉnh được nhận, đặt thành 0 sau khi xử lý.
 */
extern int command_ok;

/**
 * @brief Hàm giải mã tín hiệu IR đã nhận và lưu kết quả vào `irda_cmd`.
 * Cần được gọi sau khi `command_ok` được đặt thành 1 để xử lý lệnh mới.
 */
void IrDecode();

/**
 * @brief Hàm được gọi trong ISR của EXTI khi có tín hiệu IR mới, dùng để đo thời gian giữa các bit.
 * Cần thiết để giải mã chính xác tín hiệu IR dựa trên thời gian.
 */
void IrGetBitTime();

#endif /* SRC_IRDA_H_ */