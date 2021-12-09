/**
 * @file prefs.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Read and write settings to/from Preferences
 * @version 0.1
 * @date 2021-09-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

/** ESP32 preferences */
Preferences lora_prefs;

/**
 * @brief Get the LoRaWAN settings
 * 
 */
void get_lora_prefs(void)
{
	lora_prefs.begin("LoRaCred", false);

	bool hasPref = lora_prefs.getBool("valid", false);
	if (hasPref)
	{
		myLog_d("Found preferences");
		g_lorawan_settings.auto_join = lora_prefs.getBool("a_j", false);
		g_lorawan_settings.otaa_enabled = lora_prefs.getBool("o_e", true);
		lora_prefs.getBytes("d_e", g_lorawan_settings.node_device_eui, 8);
		lora_prefs.getBytes("a_e", g_lorawan_settings.node_app_eui, 8);
		lora_prefs.getBytes("a_k", g_lorawan_settings.node_app_key, 16);
		lora_prefs.getBytes("n_k", g_lorawan_settings.node_nws_key, 16);
		lora_prefs.getBytes("s_k", g_lorawan_settings.node_apps_key, 16);
		g_lorawan_settings.node_dev_addr = lora_prefs.getLong("d_a", 0x26021FB4);
		g_lorawan_settings.send_repeat_time = lora_prefs.getLong("s_r", 120000);
		g_lorawan_settings.adr_enabled = lora_prefs.getBool("a_d", false);
		g_lorawan_settings.public_network = lora_prefs.getBool("p_n", true);
		g_lorawan_settings.duty_cycle_enabled = lora_prefs.getBool("d_c", false);
		g_lorawan_settings.join_trials = lora_prefs.getShort("j_t", 5);
		g_lorawan_settings.tx_power = lora_prefs.getShort("t_p", 0);
		g_lorawan_settings.data_rate = lora_prefs.getShort("d_r", 5);
		g_lorawan_settings.lora_class = lora_prefs.getShort("l_c", 0);
		g_lorawan_settings.subband_channels = lora_prefs.getShort("s_c", 1);
		g_lorawan_settings.app_port = lora_prefs.getShort("a_p", 2);
		g_lorawan_settings.confirmed_msg_enabled = (lmh_confirm)lora_prefs.getShort("c_m", LMH_UNCONFIRMED_MSG);
		g_lorawan_settings.resetRequest = lora_prefs.getBool("r_r", true);
		g_lorawan_settings.lora_region = lora_prefs.getShort("l_r", 10);
		lora_prefs.end();
	}
	else
	{
		lora_prefs.end();
		myLog_d("No valid preferences");
		save_lora_settings();
	}
	log_settings();
}

/**
 * @brief Save LoRaWAN settings
 * 
 */
void save_lora_settings(void)
{
	lora_prefs.begin("LoRaCred", false);

	lora_prefs.putBool("a_j", g_lorawan_settings.auto_join);
	lora_prefs.putBool("o_e", g_lorawan_settings.otaa_enabled);
	lora_prefs.putBytes("d_e", g_lorawan_settings.node_device_eui, 8);
	lora_prefs.putBytes("a_e", g_lorawan_settings.node_app_eui, 8);
	lora_prefs.putBytes("a_k", g_lorawan_settings.node_app_key, 16);
	lora_prefs.putBytes("n_k", g_lorawan_settings.node_nws_key, 16);
	lora_prefs.putBytes("s_k", g_lorawan_settings.node_apps_key, 16);
	lora_prefs.putLong("d_a", g_lorawan_settings.node_dev_addr);
	lora_prefs.putLong("s_r", g_lorawan_settings.send_repeat_time);
	lora_prefs.putBool("a_d", g_lorawan_settings.adr_enabled);
	lora_prefs.putBool("p_n", g_lorawan_settings.public_network);
	lora_prefs.putBool("d_c", g_lorawan_settings.duty_cycle_enabled);
	lora_prefs.putShort("j_t", g_lorawan_settings.join_trials);
	lora_prefs.putShort("t_p", g_lorawan_settings.tx_power);
	lora_prefs.putShort("d_r", g_lorawan_settings.data_rate);
	lora_prefs.putShort("l_c", g_lorawan_settings.lora_class);
	lora_prefs.putShort("s_c", g_lorawan_settings.subband_channels);
	lora_prefs.putShort("a_p", g_lorawan_settings.app_port);
	lora_prefs.putShort("c_m", g_lorawan_settings.confirmed_msg_enabled);
	lora_prefs.putBool("r_r", g_lorawan_settings.resetRequest);
	lora_prefs.putShort("l_r", g_lorawan_settings.lora_region);

	lora_prefs.putBool("valid", true);
	lora_prefs.end();
}

/**
 * @brief Printout of all settings
 * 
 */
void log_settings(void)
{
	myLog_d("Saved settings:");
	myLog_d("000 Marks: %02X %02X", g_lorawan_settings.valid_mark_1, g_lorawan_settings.valid_mark_2);
	myLog_d("002 Auto join %s", g_lorawan_settings.auto_join ? "enabled" : "disabled");
	myLog_d("003 OTAA %s", g_lorawan_settings.otaa_enabled ? "enabled" : "disabled");
	myLog_d("004 Dev EUI %02X%02X%02X%02X%02X%02X%02X%02X", g_lorawan_settings.node_device_eui[0], g_lorawan_settings.node_device_eui[1],
			g_lorawan_settings.node_device_eui[2], g_lorawan_settings.node_device_eui[3],
			g_lorawan_settings.node_device_eui[4], g_lorawan_settings.node_device_eui[5],
			g_lorawan_settings.node_device_eui[6], g_lorawan_settings.node_device_eui[7]);
	myLog_d("012 App EUI %02X%02X%02X%02X%02X%02X%02X%02X", g_lorawan_settings.node_app_eui[0], g_lorawan_settings.node_app_eui[1],
			g_lorawan_settings.node_app_eui[2], g_lorawan_settings.node_app_eui[3],
			g_lorawan_settings.node_app_eui[4], g_lorawan_settings.node_app_eui[5],
			g_lorawan_settings.node_app_eui[6], g_lorawan_settings.node_app_eui[7]);
	myLog_d("020 App Key %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			g_lorawan_settings.node_app_key[0], g_lorawan_settings.node_app_key[1],
			g_lorawan_settings.node_app_key[2], g_lorawan_settings.node_app_key[3],
			g_lorawan_settings.node_app_key[4], g_lorawan_settings.node_app_key[5],
			g_lorawan_settings.node_app_key[6], g_lorawan_settings.node_app_key[7],
			g_lorawan_settings.node_app_key[8], g_lorawan_settings.node_app_key[9],
			g_lorawan_settings.node_app_key[10], g_lorawan_settings.node_app_key[11],
			g_lorawan_settings.node_app_key[12], g_lorawan_settings.node_app_key[13],
			g_lorawan_settings.node_app_key[14], g_lorawan_settings.node_app_key[15]);
	myLog_d("036 NWS Key %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			g_lorawan_settings.node_nws_key[0], g_lorawan_settings.node_nws_key[1],
			g_lorawan_settings.node_nws_key[2], g_lorawan_settings.node_nws_key[3],
			g_lorawan_settings.node_nws_key[4], g_lorawan_settings.node_nws_key[5],
			g_lorawan_settings.node_nws_key[6], g_lorawan_settings.node_nws_key[7],
			g_lorawan_settings.node_nws_key[8], g_lorawan_settings.node_nws_key[9],
			g_lorawan_settings.node_nws_key[10], g_lorawan_settings.node_nws_key[11],
			g_lorawan_settings.node_nws_key[12], g_lorawan_settings.node_nws_key[13],
			g_lorawan_settings.node_nws_key[14], g_lorawan_settings.node_nws_key[15]);
	myLog_d("052 Apps Key %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			g_lorawan_settings.node_apps_key[0], g_lorawan_settings.node_apps_key[1],
			g_lorawan_settings.node_apps_key[2], g_lorawan_settings.node_apps_key[3],
			g_lorawan_settings.node_apps_key[4], g_lorawan_settings.node_apps_key[5],
			g_lorawan_settings.node_apps_key[6], g_lorawan_settings.node_apps_key[7],
			g_lorawan_settings.node_apps_key[8], g_lorawan_settings.node_apps_key[9],
			g_lorawan_settings.node_apps_key[10], g_lorawan_settings.node_apps_key[11],
			g_lorawan_settings.node_apps_key[12], g_lorawan_settings.node_apps_key[13],
			g_lorawan_settings.node_apps_key[14], g_lorawan_settings.node_apps_key[15]);
	myLog_d("068 Dev Addr %08lX", g_lorawan_settings.node_dev_addr);
	myLog_d("072 Repeat time %ld", g_lorawan_settings.send_repeat_time);
	myLog_d("076 ADR %s", g_lorawan_settings.adr_enabled ? "enabled" : "disabled");
	myLog_d("077 %s Network", g_lorawan_settings.public_network ? "Public" : "Private");
	myLog_d("078 Dutycycle %s", g_lorawan_settings.duty_cycle_enabled ? "enabled" : "disabled");
	myLog_d("079 Join trials %d", g_lorawan_settings.join_trials);
	myLog_d("080 TX Power %d", g_lorawan_settings.tx_power);
	myLog_d("081 DR %d", g_lorawan_settings.data_rate);
	myLog_d("082 Class %d", g_lorawan_settings.lora_class);
	myLog_d("083 Subband %d", g_lorawan_settings.subband_channels);
	myLog_d("084 Fport %d", g_lorawan_settings.app_port);
	myLog_d("085 %s Message", g_lorawan_settings.confirmed_msg_enabled ? "Confirmed" : "Unconfirmed");
	myLog_d("087 Region %d", g_lorawan_settings.lora_region);
}

/**
 * @brief Pack settings for BLE transmission
 * 
 * @param buffer Buffer to write the packed data into
 * @return uint8_t number of bytes written into buffer
 */
uint8_t pack_settings(uint8_t *buffer)
{
	int i = 0;
	buffer[i++] = g_lorawan_settings.valid_mark_1;
	buffer[i++] = g_lorawan_settings.valid_mark_2;
	buffer[i++] = g_lorawan_settings.auto_join;
	buffer[i++] = g_lorawan_settings.otaa_enabled;
	buffer[i++] = g_lorawan_settings.node_device_eui[0];
	buffer[i++] = g_lorawan_settings.node_device_eui[1];
	buffer[i++] = g_lorawan_settings.node_device_eui[2];
	buffer[i++] = g_lorawan_settings.node_device_eui[3];
	buffer[i++] = g_lorawan_settings.node_device_eui[4];
	buffer[i++] = g_lorawan_settings.node_device_eui[5];
	buffer[i++] = g_lorawan_settings.node_device_eui[6];
	buffer[i++] = g_lorawan_settings.node_device_eui[7];
	buffer[i++] = g_lorawan_settings.node_app_eui[0];
	buffer[i++] = g_lorawan_settings.node_app_eui[1];
	buffer[i++] = g_lorawan_settings.node_app_eui[2];
	buffer[i++] = g_lorawan_settings.node_app_eui[3];
	buffer[i++] = g_lorawan_settings.node_app_eui[4];
	buffer[i++] = g_lorawan_settings.node_app_eui[5];
	buffer[i++] = g_lorawan_settings.node_app_eui[6];
	buffer[i++] = g_lorawan_settings.node_app_eui[7];
	buffer[i++] = g_lorawan_settings.node_app_key[0];
	buffer[i++] = g_lorawan_settings.node_app_key[1];
	buffer[i++] = g_lorawan_settings.node_app_key[2];
	buffer[i++] = g_lorawan_settings.node_app_key[3];
	buffer[i++] = g_lorawan_settings.node_app_key[4];
	buffer[i++] = g_lorawan_settings.node_app_key[5];
	buffer[i++] = g_lorawan_settings.node_app_key[6];
	buffer[i++] = g_lorawan_settings.node_app_key[7];
	buffer[i++] = g_lorawan_settings.node_app_key[8];
	buffer[i++] = g_lorawan_settings.node_app_key[9];
	buffer[i++] = g_lorawan_settings.node_app_key[10];
	buffer[i++] = g_lorawan_settings.node_app_key[11];
	buffer[i++] = g_lorawan_settings.node_app_key[12];
	buffer[i++] = g_lorawan_settings.node_app_key[13];
	buffer[i++] = g_lorawan_settings.node_app_key[14];
	buffer[i++] = g_lorawan_settings.node_app_key[15];
	buffer[i++] = g_lorawan_settings.node_nws_key[0];
	buffer[i++] = g_lorawan_settings.node_nws_key[1];
	buffer[i++] = g_lorawan_settings.node_nws_key[2];
	buffer[i++] = g_lorawan_settings.node_nws_key[3];
	buffer[i++] = g_lorawan_settings.node_nws_key[4];
	buffer[i++] = g_lorawan_settings.node_nws_key[5];
	buffer[i++] = g_lorawan_settings.node_nws_key[6];
	buffer[i++] = g_lorawan_settings.node_nws_key[7];
	buffer[i++] = g_lorawan_settings.node_nws_key[8];
	buffer[i++] = g_lorawan_settings.node_nws_key[9];
	buffer[i++] = g_lorawan_settings.node_nws_key[10];
	buffer[i++] = g_lorawan_settings.node_nws_key[11];
	buffer[i++] = g_lorawan_settings.node_nws_key[12];
	buffer[i++] = g_lorawan_settings.node_nws_key[13];
	buffer[i++] = g_lorawan_settings.node_nws_key[14];
	buffer[i++] = g_lorawan_settings.node_nws_key[15];
	buffer[i++] = g_lorawan_settings.node_apps_key[0];
	buffer[i++] = g_lorawan_settings.node_apps_key[1];
	buffer[i++] = g_lorawan_settings.node_apps_key[2];
	buffer[i++] = g_lorawan_settings.node_apps_key[3];
	buffer[i++] = g_lorawan_settings.node_apps_key[4];
	buffer[i++] = g_lorawan_settings.node_apps_key[5];
	buffer[i++] = g_lorawan_settings.node_apps_key[6];
	buffer[i++] = g_lorawan_settings.node_apps_key[7];
	buffer[i++] = g_lorawan_settings.node_apps_key[8];
	buffer[i++] = g_lorawan_settings.node_apps_key[9];
	buffer[i++] = g_lorawan_settings.node_apps_key[10];
	buffer[i++] = g_lorawan_settings.node_apps_key[11];
	buffer[i++] = g_lorawan_settings.node_apps_key[12];
	buffer[i++] = g_lorawan_settings.node_apps_key[13];
	buffer[i++] = g_lorawan_settings.node_apps_key[14];
	buffer[i++] = g_lorawan_settings.node_apps_key[15];
	buffer[i++] = (uint8_t)(g_lorawan_settings.node_dev_addr);
	buffer[i++] = (uint8_t)(g_lorawan_settings.node_dev_addr >> 8);
	buffer[i++] = (uint8_t)(g_lorawan_settings.node_dev_addr >> 16);
	buffer[i++] = (uint8_t)(g_lorawan_settings.node_dev_addr >> 24);
	buffer[i++] = (uint8_t)(g_lorawan_settings.send_repeat_time);
	buffer[i++] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 8);
	buffer[i++] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 16);
	buffer[i++] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 24);
	buffer[i++] = g_lorawan_settings.adr_enabled;
	buffer[i++] = g_lorawan_settings.public_network;
	buffer[i++] = g_lorawan_settings.duty_cycle_enabled;
	buffer[i++] = g_lorawan_settings.join_trials;
	buffer[i++] = g_lorawan_settings.tx_power;
	buffer[i++] = g_lorawan_settings.data_rate;
	buffer[i++] = g_lorawan_settings.lora_class;
	buffer[i++] = g_lorawan_settings.subband_channels;
	buffer[i++] = g_lorawan_settings.app_port;
	buffer[i++] = g_lorawan_settings.confirmed_msg_enabled;
	buffer[i++] = g_lorawan_settings.lora_region;

	return i + 1;
}

/**
 * @brief Unpack received settings
 * 
 * @param buffer Buffer with received settings
 * @return uint8_t number of bytes handled
 */
uint8_t unpack_settings(uint8_t *buffer)
{
	int i = 0;
	g_lorawan_settings.valid_mark_1 = buffer[i++];
	g_lorawan_settings.valid_mark_2 = buffer[i++];
	g_lorawan_settings.auto_join = buffer[i++];
	g_lorawan_settings.otaa_enabled = buffer[i++];
	g_lorawan_settings.node_device_eui[0] = buffer[i++];
	g_lorawan_settings.node_device_eui[1] = buffer[i++];
	g_lorawan_settings.node_device_eui[2] = buffer[i++];
	g_lorawan_settings.node_device_eui[3] = buffer[i++];
	g_lorawan_settings.node_device_eui[4] = buffer[i++];
	g_lorawan_settings.node_device_eui[5] = buffer[i++];
	g_lorawan_settings.node_device_eui[6] = buffer[i++];
	g_lorawan_settings.node_device_eui[7] = buffer[i++];
	g_lorawan_settings.node_app_eui[0] = buffer[i++];
	g_lorawan_settings.node_app_eui[1] = buffer[i++];
	g_lorawan_settings.node_app_eui[2] = buffer[i++];
	g_lorawan_settings.node_app_eui[3] = buffer[i++];
	g_lorawan_settings.node_app_eui[4] = buffer[i++];
	g_lorawan_settings.node_app_eui[5] = buffer[i++];
	g_lorawan_settings.node_app_eui[6] = buffer[i++];
	g_lorawan_settings.node_app_eui[7] = buffer[i++];
	g_lorawan_settings.node_app_key[0] = buffer[i++];
	g_lorawan_settings.node_app_key[1] = buffer[i++];
	g_lorawan_settings.node_app_key[2] = buffer[i++];
	g_lorawan_settings.node_app_key[3] = buffer[i++];
	g_lorawan_settings.node_app_key[4] = buffer[i++];
	g_lorawan_settings.node_app_key[5] = buffer[i++];
	g_lorawan_settings.node_app_key[6] = buffer[i++];
	g_lorawan_settings.node_app_key[7] = buffer[i++];
	g_lorawan_settings.node_app_key[8] = buffer[i++];
	g_lorawan_settings.node_app_key[9] = buffer[i++];
	g_lorawan_settings.node_app_key[10] = buffer[i++];
	g_lorawan_settings.node_app_key[11] = buffer[i++];
	g_lorawan_settings.node_app_key[12] = buffer[i++];
	g_lorawan_settings.node_app_key[13] = buffer[i++];
	g_lorawan_settings.node_app_key[14] = buffer[i++];
	g_lorawan_settings.node_app_key[15] = buffer[i++];
	g_lorawan_settings.node_nws_key[0] = buffer[i++];
	g_lorawan_settings.node_nws_key[1] = buffer[i++];
	g_lorawan_settings.node_nws_key[2] = buffer[i++];
	g_lorawan_settings.node_nws_key[3] = buffer[i++];
	g_lorawan_settings.node_nws_key[4] = buffer[i++];
	g_lorawan_settings.node_nws_key[5] = buffer[i++];
	g_lorawan_settings.node_nws_key[6] = buffer[i++];
	g_lorawan_settings.node_nws_key[7] = buffer[i++];
	g_lorawan_settings.node_nws_key[8] = buffer[i++];
	g_lorawan_settings.node_nws_key[9] = buffer[i++];
	g_lorawan_settings.node_nws_key[10] = buffer[i++];
	g_lorawan_settings.node_nws_key[11] = buffer[i++];
	g_lorawan_settings.node_nws_key[12] = buffer[i++];
	g_lorawan_settings.node_nws_key[13] = buffer[i++];
	g_lorawan_settings.node_nws_key[14] = buffer[i++];
	g_lorawan_settings.node_nws_key[15] = buffer[i++];
	g_lorawan_settings.node_apps_key[0] = buffer[i++];
	g_lorawan_settings.node_apps_key[1] = buffer[i++];
	g_lorawan_settings.node_apps_key[2] = buffer[i++];
	g_lorawan_settings.node_apps_key[3] = buffer[i++];
	g_lorawan_settings.node_apps_key[4] = buffer[i++];
	g_lorawan_settings.node_apps_key[5] = buffer[i++];
	g_lorawan_settings.node_apps_key[6] = buffer[i++];
	g_lorawan_settings.node_apps_key[7] = buffer[i++];
	g_lorawan_settings.node_apps_key[8] = buffer[i++];
	g_lorawan_settings.node_apps_key[9] = buffer[i++];
	g_lorawan_settings.node_apps_key[10] = buffer[i++];
	g_lorawan_settings.node_apps_key[11] = buffer[i++];
	g_lorawan_settings.node_apps_key[12] = buffer[i++];
	g_lorawan_settings.node_apps_key[13] = buffer[i++];
	g_lorawan_settings.node_apps_key[14] = buffer[i++];
	g_lorawan_settings.node_apps_key[15] = buffer[i++];
	g_lorawan_settings.node_dev_addr = ((uint32_t)(buffer[i++]));
	g_lorawan_settings.node_dev_addr = g_lorawan_settings.node_dev_addr | ((uint32_t)(buffer[i++] << 8));
	g_lorawan_settings.node_dev_addr = g_lorawan_settings.node_dev_addr | ((uint32_t)(buffer[i++] << 16));
	g_lorawan_settings.node_dev_addr = g_lorawan_settings.node_dev_addr | ((uint32_t)(buffer[i++] << 24));
	g_lorawan_settings.send_repeat_time = ((uint32_t)(buffer[i++]));
	g_lorawan_settings.send_repeat_time = g_lorawan_settings.send_repeat_time | ((uint32_t)(buffer[i++] << 8));
	g_lorawan_settings.send_repeat_time = g_lorawan_settings.send_repeat_time | ((uint32_t)(buffer[i++] << 16));
	g_lorawan_settings.send_repeat_time = g_lorawan_settings.send_repeat_time | ((uint32_t)(buffer[i++] << 24));
	g_lorawan_settings.adr_enabled = buffer[i++];
	g_lorawan_settings.public_network = buffer[i++];
	g_lorawan_settings.duty_cycle_enabled = buffer[i++];
	g_lorawan_settings.join_trials = buffer[i++];
	g_lorawan_settings.tx_power = buffer[i++];
	g_lorawan_settings.data_rate = buffer[i++];
	g_lorawan_settings.lora_class = buffer[i++];
	g_lorawan_settings.subband_channels = buffer[i++];
	g_lorawan_settings.app_port = buffer[i++];
	g_lorawan_settings.confirmed_msg_enabled = (lmh_confirm)buffer[i++];
	g_lorawan_settings.lora_region = buffer[87];

	return i + 1;
}

/**
 * @brief Get stored data from the preferences
 */
void get_wifi_prefs(void)
{
	Preferences preferences;
	preferences.begin("WiFiCred", false);
	bool hasPref = preferences.getBool("valid", false);
	if (hasPref)
	{
		g_ssid_prim = preferences.getString("g_ssid_prim", "");
		g_ssid_sec = preferences.getString("g_ssid_sec", "");
		g_pw_prim = preferences.getString("g_pw_prim", "");
		g_pw_sec = preferences.getString("g_pw_sec", "");

		if (g_ssid_prim.equals("") || g_pw_prim.equals("") || g_ssid_sec.equals("") || g_pw_prim.equals(""))
		{
			myLog_e("Found preferences but credentials are invalid");
		}
		else
		{
			myLog_d("Preferences: primary SSID: %s pw: %s secondary SSID: %s pw: %s",
					g_ssid_prim.c_str(), g_pw_prim.c_str(),
					g_ssid_sec.c_str(), g_pw_sec.c_str());
			g_has_credentials = true;
		}

		IPAddress newIP(0, 0, 0, 0);
		uint8_t partIP = 0;

		newIP[0] = preferences.getShort("sma_0", 0);
		newIP[1] = preferences.getShort("sma_1", 0);
		newIP[2] = preferences.getShort("sma_2", 0);
		newIP[3] = preferences.getShort("sma_3", 0);

		if ((newIP[0] == 0) && (newIP[1] == 0) && (newIP[2] == 0) && (newIP[3] == 0))
		{
			myLog_d("No valid SMA IP found, use default");
		}
		else
		{
			inverterIP = newIP;
		}
	}
	else
	{
		myLog_d("Could not find preferences, need send data over BLE");
	}
	preferences.end();

	/// \todo Optionial, here you can set hard-coded WiFi credentials
	// if (!g_has_credentials)
	// {
	// 	myLog_d("Creating default preferences");
	// 	preferences.begin("WiFiCred", false);
	// 	preferences.putBool("valid", true);
	// 	preferences.putString("g_ssid_prim", "YOUR_WIFI_NETWORK_AP");
	// 	preferences.putString("g_ssid_sec", "YOUR_WIFI_NETWORK_AP");
	// 	preferences.putString("g_pw_prim", "YOUR_WIFI_NETWORK_PASSWORD");
	// 	preferences.putString("g_pw_sec", "YOUR_WIFI_NETWORK_PASSWORD");

	// 	g_ssid_prim = preferences.getString("g_ssid_prim", "");
	// 	g_ssid_sec = preferences.getString("g_ssid_sec", "");
	// 	g_pw_prim = preferences.getString("g_pw_prim", "");
	// 	g_pw_sec = preferences.getString("g_pw_sec", "");

	// 	preferences.end();
	// 	g_has_credentials = true;
	// }
}

void saveSmaIP(void)
{
	Preferences preferences;
	preferences.begin("WiFiCred", false);
	preferences.putShort("sma_0", inverterIP[0]);
	preferences.putShort("sma_1", inverterIP[1]);
	preferences.putShort("sma_2", inverterIP[2]);
	preferences.putShort("sma_3", inverterIP[3]);
	preferences.end();
}