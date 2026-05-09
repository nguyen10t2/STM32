#ifndef __DS1307_H
#define __DS1307_H

#include <stdint.h>

// Cấu trúc dữ liệu để lưu trữ thời gian
typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} DS1307_TimeTypeDef;

// Hàm khởi tạo DS1307 (khởi tạo 1 lần duy nhất)
void DS1307_Init(void);

// Hàm thiết lập thời gian cho DS1307
void SetTime(DS1307_TimeTypeDef *time);

// Hàm đọc thời gian từ DS1307
void GetTime(DS1307_TimeTypeDef *time);

void TestTime(void);

#endif /* __DS1307_H */