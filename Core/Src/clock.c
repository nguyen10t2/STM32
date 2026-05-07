#include "clock.h"
#include "SH1106.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static uint32_t clock_last_tick = 0;
static uint32_t start_tick = 0;

void Clock_Init(void)
{
	start_tick = HAL_GetTick();
	clock_last_tick = start_tick;
	SH1106_Fill(SH1106_COLOR_BLACK);
	SH1106_UpdateScreen();
}

void Clock_Task(void)
{
	uint32_t now = HAL_GetTick();
	
	if (now - clock_last_tick >= 1000)
	{
		clock_last_tick = now;
		
		uint32_t elapsed_seconds = now / 1000;
		// Let's assume we start at 12:00:00 for demo purposes, or use total_seconds
		uint8_t hours = (elapsed_seconds / 3600) % 12;
		uint8_t minutes = (elapsed_seconds / 60) % 60;
		uint8_t seconds = elapsed_seconds % 60;
		
		SH1106_Fill(SH1106_COLOR_BLACK);
		
		// Draw clock face
		int16_t center_x = 64;
		int16_t center_y = 32;
		int16_t radius = 31;
		
		SH1106_DrawCircle(center_x, center_y, radius, SH1106_COLOR_WHITE);
        
        // Draw tick marks
        for(int i=0; i<12; i++) {
            float angle = i * M_PI / 6.0;
            int16_t x1 = center_x + sin(angle) * (radius - 4);
            int16_t y1 = center_y - cos(angle) * (radius - 4);
            int16_t x2 = center_x + sin(angle) * radius;
            int16_t y2 = center_y - cos(angle) * radius;
            SH1106_DrawLine(x1, y1, x2, y2, SH1106_COLOR_WHITE);
        }
		
		// Calculate angles
		float sec_angle = (seconds * M_PI / 30.0);
		float min_angle = (minutes * M_PI / 30.0) + (seconds * M_PI / 1800.0);
		float hour_angle = (hours * M_PI / 6.0) + (minutes * M_PI / 360.0);
		
		// Draw Hour hand
		int16_t h_len = 16;
		int16_t h_x = center_x + sin(hour_angle) * h_len;
		int16_t h_y = center_y - cos(hour_angle) * h_len;
		SH1106_DrawLine(center_x, center_y, h_x, h_y, SH1106_COLOR_WHITE);
		
		// Draw Minute hand
		int16_t m_len = 24;
		int16_t m_x = center_x + sin(min_angle) * m_len;
		int16_t m_y = center_y - cos(min_angle) * m_len;
		SH1106_DrawLine(center_x, center_y, m_x, m_y, SH1106_COLOR_WHITE);
		
		// Draw Second hand
		int16_t s_len = 28;
		int16_t s_x = center_x + sin(sec_angle) * s_len;
		int16_t s_y = center_y - cos(sec_angle) * s_len;
		SH1106_DrawLine(center_x, center_y, s_x, s_y, SH1106_COLOR_WHITE);
		
		SH1106_UpdateScreen();
	}
}
