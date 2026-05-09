#ifndef __TASK_H
#define __TASK_H

#define MAX_LOG_SIZE 100

extern const char* welcome;
extern const char* reject;

#include "stdint.h"

// Khởi tạo các task
void Task_Init(void);

// Đọc thẻ RFID
uint8_t* Read_RFID(void);

// Gửi dữ liệu qua UART
void Send_UART(uint8_t* data, uint16_t size);

// Lưu log trên RAM tối đa MAX_LOG_SIZE
void Log_Data(uint8_t* data, uint16_t size);

void Task_Run(void);

#endif /* __TASK_H */