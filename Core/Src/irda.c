/*
* irda.c
*
* IR Remote Control Decoder - NEC Protocol
*
* Nguyen ly hoat dong:
* - Timer6 chay o 10000 Hz (0.1ms/tick)
* - Moi khi co falling edge tren PG5 (HS0038 output), ngat EXTI5 duoc goi
* - Ham IrGetBitTime() ghi lai so tick giua 2 lan ngat lien tiep
* - Neu khoang cach > 100 tick (>10ms) => bit Start (9ms burst + 4.5ms space = 13.5ms)
* - Neu khoang cach ~ 11-12 tick (~1.125ms) => bit 0
* - Neu khoang cach ~ 22-23 tick (~2.25ms)  => bit 1
* - Sau khi nhan du 33 bit (1 start + 32 data), command_ok = 1
* - Ham IrDecode() giai ma 32 bit thanh 4 byte: addr, ~addr, cmd, ~cmd
*/
#include "irda.h"
#include "stm32f4xx_hal.h"
 
extern int tim6_count;
 
int bit_time[33];
int bit_count;
int command_ok;
unsigned char irda_cmd[4];
 
void IrGetBitTime()
{
    // Neu thoi gian > 100 tick (>10ms) => day la bit Start
    // Reset bo dem bit
    if (tim6_count > 100)
    {
        bit_count = 0;
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
    }
 
    // Ghi lai thoi gian cua bit hien tai
    bit_time[bit_count] = tim6_count;
    tim6_count = 0;
    bit_count++;
 
    // Da nhan du 33 bit (1 start + 32 data)
    if (bit_count == 33)
    {
        bit_count = 0;
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
        command_ok = 1;
    }
}
 
void IrDecode()
{
    unsigned char code;
    int bit, key, index = 0;
 
    // Giai ma 4 byte (32 bit) tu mang bit_time
    // bit_time[0] la bit Start, bo qua
    // bit_time[1..32] la 32 bit data
    for (key = 0; key < 4; key++)
    {
        code = 0;
        for (bit = 0; bit < 8; bit++)
        {
            code >>= 1;
            index++;
            // Neu thoi gian > 20 tick (>2ms) => bit 1
            // Neu thoi gian <= 20 tick (~1.125ms) => bit 0
            if (bit_time[index] > 20)
                code |= 0x80;
        }
        irda_cmd[key] = code;
    }
    // irda_cmd[0] = Address
    // irda_cmd[1] = ~Address (complement)
    // irda_cmd[2] = Command
    // irda_cmd[3] = ~Command (complement)
}