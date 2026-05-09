#include "task.h"
#include <stdio.h>
#include <string.h>
#include "tm_stm32f4_mfrc522.h"
#include "SH1106.h"
#include "ds1307.h"
#include "gpio.h"
#include "usart.h"

const char* welcome = "Welcome";
const char* reject = "Reject";

#define MAX_VALID_CARDS 20

typedef struct {
    uint8_t year, month, date, hours, minutes, seconds;
    uint8_t card_id[5];
} LogRecord;

LogRecord access_logs[MAX_LOG_SIZE];
uint16_t log_count = 0;

uint8_t valid_cards[MAX_VALID_CARDS][5];
uint16_t valid_cards_count = 0;

extern char rx_buf[100];
extern uint8_t rx_idx;
extern volatile uint8_t cmd_received;

uint8_t pre_card_status = 0;
static uint8_t CheckValidCard(uint8_t* card_id);

void Task_Init(void)
{
    SH1106_Init();
    DS1307_Init();
    TM_MFRC522_Init();
    
    SH1106_GotoXY(12, 10);
    SH1106_Puts("Hello Lam", &Font_11x18, 1);
    SH1106_UpdateScreen();
}

uint8_t hex2int(char c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 'A' + 10);
    }
    return 0xFF;
}

void ProcessCommand(char* cmd)
{
    uint8_t card_id[5];
    char* cursor;
    char response[48];

    if (cmd == NULL) {
        return;
    }

    while (*cmd == ' ') {
        cmd++;
    }

    {
        size_t len = strlen(cmd);
        while (len > 0 && (cmd[len - 1] == '\r' || cmd[len - 1] == '\n' || cmd[len - 1] == ' ')) {
            cmd[--len] = '\0';
        }
    }

    if (strncmp(cmd, "ADD", 3) == 0) {
        if (valid_cards_count >= MAX_VALID_CARDS) {
            Send_UART((uint8_t*)"ERR FULL\r\n", 10);
            return;
        }

        cursor = cmd + 3;
        while (*cursor == ' ' || *cursor == ':' || *cursor == '=') {
            cursor++;
        }

        if (strlen(cursor) < 10) {
            Send_UART((uint8_t*)"ERR FORMAT\r\n", 12);
            return;
        }

        for (int i = 0; i < 5; i++) {
            uint8_t hi = hex2int(cursor[i * 2]);
            uint8_t lo = hex2int(cursor[i * 2 + 1]);
            if (hi == 0xFF || lo == 0xFF) {
                Send_UART((uint8_t*)"ERR HEX\r\n", 9);
                return;
            }
            card_id[i] = (uint8_t)((hi << 4) | lo);
        }

        if (CheckValidCard(card_id)) {
            Send_UART((uint8_t*)"ERR EXISTS\r\n", 12);
            return;
        }

        memcpy(valid_cards[valid_cards_count], card_id, 5);
        valid_cards_count++;
        Send_UART((uint8_t*)"OK ADD\r\n", 8);
        return;
    }

    if (strncmp(cmd, "DEL", 3) == 0) {
        cursor = cmd + 3;
        while (*cursor == ' ' || *cursor == ':' || *cursor == '=') {
            cursor++;
        }

        if (strlen(cursor) < 10) {
            Send_UART((uint8_t*)"ERR FORMAT\r\n", 12);
            return;
        }

        for (int i = 0; i < 5; i++) {
            uint8_t hi = hex2int(cursor[i * 2]);
            uint8_t lo = hex2int(cursor[i * 2 + 1]);
            if (hi == 0xFF || lo == 0xFF) {
                Send_UART((uint8_t*)"ERR HEX\r\n", 9);
                return;
            }
            card_id[i] = (uint8_t)((hi << 4) | lo);
        }

        for (int i = 0; i < valid_cards_count; i++) {
            if (TM_MFRC522_Compare(card_id, valid_cards[i]) == MI_OK) {
                for (int j = i + 1; j < valid_cards_count; j++) {
                    memcpy(valid_cards[j - 1], valid_cards[j], 5);
                }
                valid_cards_count--;
                Send_UART((uint8_t*)"OK DEL\r\n", 8);
                return;
            }
        }

        Send_UART((uint8_t*)"ERR NOTFOUND\r\n", 14);
        return;
    }

    if (strcmp(cmd, "LIST") == 0) {
        sprintf(response, "COUNT %u\r\n", valid_cards_count);
        Send_UART((uint8_t*)response, strlen(response));
        return;
    }

    Send_UART((uint8_t*)"ERR CMD\r\n", 9);
}

uint8_t* Read_RFID(void)
{
    static uint8_t CardID[5];
    if (TM_MFRC522_Check(CardID) == MI_OK) {
        return CardID;
    }
    return NULL;
}

void Send_UART(uint8_t* data, uint16_t size)
{
    HAL_UART_Transmit(&huart1, data, size, 100);
}

void Log_Data(uint8_t* data, uint16_t size)
{
    if (log_count < MAX_LOG_SIZE && size == 5) {
        DS1307_TimeTypeDef current_time;
        GetTime(&current_time);

        access_logs[log_count].year = current_time.year;
        access_logs[log_count].month = current_time.month;
        access_logs[log_count].date = current_time.date;
        access_logs[log_count].hours = current_time.hour;
        access_logs[log_count].minutes = current_time.min;
        access_logs[log_count].seconds = current_time.sec;

        for (int i = 0; i < 5; i++) {
            access_logs[log_count].card_id[i] = data[i];
        }
        log_count++;
    } else if (size == 5) {
        for(int i = 1; i < MAX_LOG_SIZE; i++) {
            access_logs[i-1] = access_logs[i];
        }
        DS1307_TimeTypeDef current_time;
        GetTime(&current_time);

        access_logs[MAX_LOG_SIZE-1].year = current_time.year;
        access_logs[MAX_LOG_SIZE-1].month = current_time.month;
        access_logs[MAX_LOG_SIZE-1].date = current_time.date;
        access_logs[MAX_LOG_SIZE-1].hours = current_time.hour;
        access_logs[MAX_LOG_SIZE-1].minutes = current_time.min;
        access_logs[MAX_LOG_SIZE-1].seconds = current_time.sec;

        for (int i = 0; i < 5; i++) {
            access_logs[MAX_LOG_SIZE-1].card_id[i] = data[i];
        }
    }
}

static uint8_t CheckValidCard(uint8_t* card_id) {
    for (int i = 0; i < valid_cards_count; i++) {
        if (TM_MFRC522_Compare(card_id, valid_cards[i]) == MI_OK) {
            return 1;
        }
    }
    return 0;
}

void Task_Run(void)
{
    if (cmd_received) {
        char debug_buf[50];
        sprintf(debug_buf, "Received: %s\r\n", rx_buf);
        Send_UART((uint8_t*)debug_buf, strlen(debug_buf));
        
        ProcessCommand(rx_buf);
        cmd_received = 0;
        rx_idx = 0;
    }

    uint8_t* CardID = Read_RFID();
    char buf[50] = "\000";

    if (CardID != NULL) {
        if (pre_card_status == 0) {
            pre_card_status = 1;
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET); // LED3 bật

            if (CheckValidCard(CardID)) {
                sprintf(buf, "%s", welcome);
                HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET); // LED4 bật
                Log_Data(CardID, 5);
            } else {
                sprintf(buf, "%s", reject);
                HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
            }
            SH1106_GotoXY(12, 10);
            SH1106_Clear();
            SH1106_Puts(buf, &Font_11x18, 1);
            SH1106_UpdateScreen();
        }
    } else {
        if (pre_card_status == 1) {
            pre_card_status = 0;
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET); // Cả LED3, LED4 tắt 
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
            
            sprintf(buf, "%s", "Hello Lam");
            SH1106_GotoXY(12, 10);
            SH1106_Puts(buf, &Font_11x18, 1);
            SH1106_UpdateScreen();
        }
    }
}
