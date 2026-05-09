#include "ds1307.h"
#include "i2c.h"
#include <stdio.h>

// Địa chỉ I2C của DS1307 (dịch trái 1 bit cho thư viện HAL: 0x68 << 1)
#define DS1307_I2C_ADDR 0xD0

// Khai báo handle I2C sử dụng cho module DS1307
extern I2C_HandleTypeDef hi2c3;

// Khai báo handle UART để in debug (nếu cần)
extern UART_HandleTypeDef huart1;

// Hàm chuyển đổi từ số thập phân sang BCD
static uint8_t decToBcd(int val) {
    return (uint8_t)( (val/10*16) + (val%10) );
}

// Hàm chuyển đổi từ BCD sang số thập phân
static int bcdToDec(uint8_t val) {
    return (int)( (val/16*10) + (val%16) );
}

void DS1307_Init(void) {
    // Đọc thanh ghi giây (Register 0)
    uint8_t sec = 0;
    HAL_I2C_Mem_Read(&hi2c3, DS1307_I2C_ADDR, 0x00, 1, &sec, 1, 1000);
    
    // Nếu bit CH (Clock Halt - bit 7) đang bật, tắt đi để bắt đầu đếm giờ
    if ((sec & 0x80) != 0) {
        sec &= 0x7F; // Xoá bit 7
        HAL_I2C_Mem_Write(&hi2c3, DS1307_I2C_ADDR, 0x00, 1, &sec, 1, 1000);
    }
}

void SetTime(DS1307_TimeTypeDef *time) {
    uint8_t data[7];
    
    data[0] = decToBcd(time->sec);
    data[1] = decToBcd(time->min);
    data[2] = decToBcd(time->hour);
    data[3] = decToBcd(time->day);
    data[4] = decToBcd(time->date);
    data[5] = decToBcd(time->month);
    data[6] = decToBcd(time->year);
    
    // Ghi 7 byte vào các thanh ghi từ 0x00 đến 0x06
    HAL_I2C_Mem_Write(&hi2c3, DS1307_I2C_ADDR, 0x00, 1, data, 7, 1000);
}

void GetTime(DS1307_TimeTypeDef *time) {
    uint8_t data[7];
    
    // Đọc 7 byte từ thanh ghi 0x00 đến 0x06
    HAL_I2C_Mem_Read(&hi2c3, DS1307_I2C_ADDR, 0x00, 1, data, 7, 1000);
    
    time->sec   = bcdToDec(data[0] & 0x7F); // Bỏ qua bit CH (bit 7)
    time->min   = bcdToDec(data[1]);
    time->hour  = bcdToDec(data[2] & 0x3F); // Bỏ qua định dạng 12/24h (ở đây mặc định 24h)
    time->day   = bcdToDec(data[3]);
    time->date  = bcdToDec(data[4]);
    time->month = bcdToDec(data[5]);
    time->year  = bcdToDec(data[6]);
}

void TestTime(void) {
    char buffer[30];
    DS1307_TimeTypeDef setTime = { .sec = 10, .min = 30, .hour = 7, .day = 7, .month = 5, .year = 26, .date = 9};
    DS1307_TimeTypeDef getTime;
    SetTime(&setTime);
    while (1) {
        GetTime(&getTime);
        sprintf(buffer, "20%02d/%02d/%02d %02d:%02d:%02d\r\n", getTime.year, getTime.month, getTime.date, getTime.hour, getTime.min, getTime.sec);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
        HAL_Delay(1000);
    }
}
