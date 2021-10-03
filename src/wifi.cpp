/**
 * @file wifi.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief WiFi initialisation and callback handlers
 * @version 0.1
 * @date 2020-11-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "main.h"

/** Unique device name */
char g_ap_name[] = "MHC-SMA-XXXXXXXXXXXXXXXX";
// const char *devAddr;
uint8_t devAddrArray[8];

/** Selected network 
    true = use primary network
	false = use secondary network
*/
bool usePrimAP = true;
/** Flag if stored AP credentials are available */
bool g_has_credentials = false;
/** Connection status */
volatile bool g_wifi_connected = false;
/** Connection change status */
bool g_conn_status_changed = false;

/** Multi WiFi */
WiFiMulti wifi_multi;

/** Primary SSID of local WiFi network */
String g_ssid_prim;
/** Secondary SSID of local WiFi network */
String g_ssid_sec;
/** Password for primary local WiFi network */
String g_pw_prim;
/** Password for secondary local WiFi network */
String g_pw_sec;

/**
 * @briefCallback for WiFi events
 */
void wifi_event_cb(WiFiEvent_t event)
{
	myLog_d("[WiFi-event] event: %d", event);
	IPAddress localIP;
	switch (event)
	{
	case SYSTEM_EVENT_STA_GOT_IP:
		g_conn_status_changed = true;

		localIP = WiFi.localIP();
		myLog_d("Connected to AP: %s with IP: %d.%d.%d.%d RSSI: %d",
				WiFi.SSID().c_str(),
				localIP[0], localIP[1], localIP[2], localIP[3],
				WiFi.RSSI());
		g_wifi_connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		g_conn_status_changed = true;
		myLog_d("WiFi lost connection");
		g_wifi_connected = false;
		break;
	case SYSTEM_EVENT_SCAN_DONE:
		myLog_v("WiFi scan finished");
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		myLog_d("WiFi STA connected");
		break;
	case SYSTEM_EVENT_WIFI_READY:
		myLog_v("WiFi interface ready");
		break;
	case SYSTEM_EVENT_STA_START:
		myLog_v("WiFi client started");
		break;
	case SYSTEM_EVENT_STA_STOP:
		myLog_v("WiFi clients stopped");
		break;
	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
		myLog_v("Authentication mode of access point has changed");
		break;
	case SYSTEM_EVENT_STA_LOST_IP:
		myLog_d("Lost IP address and IP address is reset to 0");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
		myLog_v("WiFi Protected Setup (WPS): succeeded in enrollee mode");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_FAILED:
		myLog_e("WiFi Protected Setup (WPS): failed in enrollee mode");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
		myLog_v("WiFi Protected Setup (WPS): timeout in enrollee mode");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_PIN:
		myLog_v("WiFi Protected Setup (WPS): pin code in enrollee mode");
		break;
	default:
		break;
	}
}

/**
 * Create unique device name from MAC address
 **/
void create_dev_name(void)
{
	// Get MAC address for WiFi station
	esp_wifi_get_mac(WIFI_IF_STA, devAddrArray);

	// Write unique name into g_ap_name
	sprintf(g_ap_name, "MHC-SMA-%02X%02X%02X%02X%02X%02X",
			devAddrArray[0], devAddrArray[1],
			devAddrArray[2], devAddrArray[3],
			devAddrArray[4], devAddrArray[5]);
	myLog_d("Device name: %s", g_ap_name);
}

/**
 * Initialize WiFi 
 * - Check if WiFi credentials are stored in the preferences
 * - Create unique device name
 * - Register WiFi event callback function
 * - Try to connect to WiFi if credentials are available
 */
void init_wifi(void)
{
	get_wifi_prefs();

	if (!g_has_credentials)
	{
		return;
	}

	WiFi.disconnect(true);
	delay(100);
	WiFi.enableSTA(true);
	delay(100);
	WiFi.mode(WIFI_STA);
	delay(100);
	WiFi.onEvent(wifi_event_cb);

	create_dev_name();

	if (g_has_credentials)
	{
		// Using WiFiMulti to connect to best AP
		wifi_multi.addAP(g_ssid_prim.c_str(), g_pw_prim.c_str());
		wifi_multi.addAP(g_ssid_sec.c_str(), g_pw_sec.c_str());

		wifi_multi.run();
	}
}
