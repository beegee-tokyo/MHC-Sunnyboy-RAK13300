/**
 * @file main.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Includes and defines
 * @version 0.1
 * @date 2021-08-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <AsyncUDP.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <SPI.h>
#include <Esp.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <my-log.h>
#include <ArduinoJson.h>
#include <SMAReader.h>

// WiFi stuff
#define WLANSSID "MyHomeLan"
#define WLANPWD "Bernabe@1700"
#define INVERTERPWD "Philergy21!"

// SX1262
#include "LoRaWan-Arduino.h"

int8_t init_lorawan(void);
lmh_error_status send_lora_packet(uint8_t *data, uint8_t size);

#define LORAWAN_DATA_MARKER 0x57

struct s_lorawan_settings
{
	uint8_t valid_mark_1 = 0xAA;				// Just a marker for the Flash
	uint8_t valid_mark_2 = LORAWAN_DATA_MARKER; // Just a marker for the Flash

	// Flag if node joins automatically after reboot
	bool auto_join = false;
	// Flag for OTAA or ABP
	bool otaa_enabled = true;
	// OTAA Device EUI MSB
	uint8_t node_device_eui[8] = {0x00, 0x0D, 0x75, 0xE6, 0x56, 0x4D, 0xC1, 0xF3};
	// OTAA Application EUI MSB
	uint8_t node_app_eui[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x01, 0xE1};
	// OTAA Application Key MSB
	uint8_t node_app_key[16] = {0x2B, 0x84, 0xE0, 0xB0, 0x9B, 0x68, 0xE5, 0xCB, 0x42, 0x17, 0x6F, 0xE7, 0x53, 0xDC, 0xEE, 0x79};
	// ABP Network Session Key MSB
	uint8_t node_nws_key[16] = {0x32, 0x3D, 0x15, 0x5A, 0x00, 0x0D, 0xF3, 0x35, 0x30, 0x7A, 0x16, 0xDA, 0x0C, 0x9D, 0xF5, 0x3F};
	// ABP Application Session key MSB
	uint8_t node_apps_key[16] = {0x3F, 0x6A, 0x66, 0x45, 0x9D, 0x5E, 0xDC, 0xA6, 0x3C, 0xBC, 0x46, 0x19, 0xCD, 0x61, 0xA1, 0x1E};
	// ABP Device Address MSB
	uint32_t node_dev_addr = 0x26021FB4;
	// Send repeat time in milliseconds: 2 * 60 * 1000 => 2 minutes
	uint32_t send_repeat_time = 120000;
	// Flag for ADR on or off
	bool adr_enabled = false;
	// Flag for public or private network
	bool public_network = true;
	// Flag to enable duty cycle (validity depends on Region)
	bool duty_cycle_enabled = false;
	// Number of join retries
	uint8_t join_trials = 5;
	// TX power 0 .. 15 (validity depends on Region)
	uint8_t tx_power = 0;
	// Data rate 0 .. 15 (validity depends on Region)
	uint8_t data_rate = 5;
	// LoRaWAN class 0: A, 2: C, 1: B is not supported
	uint8_t lora_class = 0;
	// Subband channel selection 1 .. 9
	uint8_t subband_channels = 1;
	// Data port to send data
	uint8_t app_port = 2;
	// Flag to enable confirmed messages
	lmh_confirm confirmed_msg_enabled = LMH_UNCONFIRMED_MSG;
	// Command from BLE to reset device
	bool resetRequest = true;
	// LoRa region
	uint8_t lora_region = 10;
};

// int size = sizeof(s_lorawan_settings);
extern s_lorawan_settings g_lorawan_settings;
extern bool g_lpwan_has_joined;
extern bool g_lorawan_initialized;
extern int16_t g_last_rssi;
extern int8_t g_last_snr;
extern uint32_t otaaDevAddr;

void initOTA();
extern bool g_ota_running;

void init_wifi(void);
void get_wifi_prefs(void);
void saveSmaIP(void);
extern bool g_has_credentials;
extern bool g_conn_status_changed;
extern volatile bool g_wifi_connected;
extern WiFiMulti wifi_multi;
extern char g_ap_name[];
extern String g_ssid_prim;
extern String g_ssid_sec;
extern String g_pw_prim;
extern String g_pw_sec;
extern IPAddress inverterIP;
extern AsyncUDP udp;

// Includes for BLE
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertising.h>
#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>

void init_ble();
void start_ble_adv(void);
void stop_ble_adv(void);
extern bool g_ble_uart_is_connected;
extern BLECharacteristic *uart_tx_characteristic;

#define BLE_PRINTF(...)                                                 \
	if (g_ble_uart_is_connected)                                        \
	{                                                                   \
		char buff[255];                                                 \
		int len = sprintf(buff, __VA_ARGS__);                           \
		uart_tx_characteristic->setValue((uint8_t *)buff, (size_t)len); \
		uart_tx_characteristic->notify(true);                           \
	}

// Preferences stuff
void get_lora_prefs(void);
void save_lora_settings(void);
void log_settings(void);
uint8_t pack_settings(uint8_t *buffer);
uint8_t unpack_settings(uint8_t *buffer);

void api_set_version(uint16_t sw_1 = 1, uint16_t sw_2 = 0, uint16_t sw_3 = 0);
void api_read_credentials(void);
void api_set_credentials(void);
extern uint16_t g_sw_ver_1; // major version increase on API change / not backwards compatible
extern uint16_t g_sw_ver_2; // minor version increase on API change / backward compatible
extern uint16_t g_sw_ver_3; // patch version increase on bugfix, no affect on API

// Battery stuff
void init_batt(void);
float read_batt(void);
uint8_t mv_to_percent(float mvolts);
uint8_t get_lora_batt(void);

// AT Command stuff
void at_serial_input(uint8_t cmd);
void start_check_serial(void);

// Display stuff
#include <SSD1306Wire.h>
void init_display(void);
void write_display(int power, int collected);
extern bool isSuccess;

// Solar Panel stuff
union value_s
{
	uint16_t val_16 = 0;
	uint8_t val_8[2];
};
extern SMAReader smaReader;