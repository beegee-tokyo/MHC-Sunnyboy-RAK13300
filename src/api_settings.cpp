/**
 * @file api_settings.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief API settings (copy of WisBlock API)
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

// Firmware version, can be set here or by API command
/** major version increase on API change / not backwards compatible */
uint16_t g_sw_ver_1 = 1;
/** minor version increase on API change / backward compatible */
uint16_t g_sw_ver_2 = 0;
/** patch version increase on bugfix, no affect on API */
uint16_t g_sw_ver_3 = 0; 

/**
 * @brief Set application version
 * 
 * @param sw_1 SW version major number
 * @param sw_2 SW version minor number
 * @param sw_3 SW version patch number
 */
void api_set_version(uint16_t sw_1, uint16_t sw_2, uint16_t sw_3)
{
	g_sw_ver_1 = sw_1;
	g_sw_ver_2 = sw_2;
	g_sw_ver_3 = sw_3;
}

/**
 * @brief Inform API that hard coded LoRaWAN credentials are used
 * 
 */
void api_set_credentials(void)
{
	save_lora_settings();
}

/**
 * @brief Force reading the LoRaWAN credentials
 * 
 */
void api_read_credentials(void)
{
	get_lora_prefs();
}
