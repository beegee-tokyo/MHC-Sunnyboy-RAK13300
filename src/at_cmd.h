/**
 * @file at_cmd.h
 * @author Taylor Lee (taylor.lee@rakwireless.com)
 * @brief AT command parsing includes & defines
 * @version 0.1
 * @date 2021-04-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

#ifndef __AT_H__
#define __AT_H__

#define AT_PRINTF(...)           \
	Serial.printf(__VA_ARGS__);  \
	if (g_ble_uart_is_connected) \
	{                            \
		BLE_PRINTF(__VA_ARGS__); \
	}

#define BLE_PRINTF(...)                                                 \
	if (g_ble_uart_is_connected)                                        \
	{                                                                   \
		char buff[255];                                                 \
		int len = sprintf(buff, __VA_ARGS__);                           \
		uart_tx_characteristic->setValue((uint8_t *)buff, (size_t)len); \
	}
#endif
