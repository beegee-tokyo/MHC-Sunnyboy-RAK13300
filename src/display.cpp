/**
 * @file display.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Display handling
 * @version 0.1
 * @date 2021-10-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"
#include "my_font.h"

/** oled_disp instance **/
SSD1306Wire oled_disp(0x3c, SDA, SCL);

/** Buffer for line */
char disp_line[32] = {0};

/** Alignment of text */
uint8_t set_alignment = TEXT_ALIGN_LEFT;

void write_header(void);

void init_display(void)
{
	oled_disp.init();
	// oled_disp.flipScreenVertically(); // clear the internal memory

	oled_disp.setFont(Monospaced_bold_10);
	snprintf(disp_line, sizeof(disp_line), "SMA Sunnyboy");
	oled_disp.drawString(0, 0, disp_line);
	oled_disp.display(); // transfer internal memory to the display
}

void write_header(void)
{
	oled_disp.setFont(Monospaced_bold_10);
	char data[32] = {0};
	memset(data, 0, sizeof(data));
	snprintf(data, sizeof(data), "SMA Sunnyboy");
	oled_disp.drawString(0, 0, data);
}

void write_display(int power, int collected)
{
	oled_disp.clear();
	uint8_t x_pos = 0;
	// TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT
	switch (set_alignment)
	{
	case 0:
		oled_disp.setTextAlignment(TEXT_ALIGN_LEFT);
		x_pos = 0;
		set_alignment = 1;
		break;
	case 1:
		oled_disp.setTextAlignment(TEXT_ALIGN_CENTER);
		x_pos = 64;
		set_alignment = 2;
		break;
	default:
		oled_disp.setTextAlignment(TEXT_ALIGN_RIGHT);
		x_pos = 127;
		set_alignment = 0;
		break;
	}
	oled_disp.setFont(Monospaced_bold_10);
	snprintf(disp_line, sizeof(disp_line), "SMA Sunnyboy %s", isSuccess ? "O" : "X");
	oled_disp.drawString(x_pos, 0, disp_line);

	oled_disp.setFont(Monospaced_bold_15);
	snprintf(disp_line, sizeof(disp_line), "P: %dW", power);
	oled_disp.drawString(x_pos, 16, disp_line);

	snprintf(disp_line, sizeof(disp_line), "E: %dWh", collected);
	oled_disp.drawString(x_pos, 38, disp_line);
	oled_disp.display();
}
