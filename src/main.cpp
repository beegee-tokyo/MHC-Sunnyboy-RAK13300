/**
 * Example: SMAReader_Demo.ino
 *
 */
#include "main.h"

/** Address of SMA Sunnyboy Inverter */
IPAddress inverterIP(192, 168, 1, 82);

/** Async UDP */
AsyncUDP udp;

/** Network address mask for UDP multicast messaging */
IPAddress multiIP(192, 168, 1, 255);

/** UDP broadcast port */
int udpBcPort = 9997;

/** SMA value reader */
SMAReader smaReader(inverterIP, SMAREADER_USER, INVERTERPWD, 5);

/** SW version of the gateway */
char g_sw_version[10];

/** WiFiUDP class for creating UDP communication */
WiFiUDP udpClientServer;

/**
 * @brief Arduino setup
 * 
 */
void setup()
{
	Serial.begin(115200);
	pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	digitalWrite(LED_GREEN, HIGH);
	digitalWrite(LED_BLUE, HIGH);

	start_check_serial();

	init_batt();

	init_wifi();
	if (g_has_credentials)
	{
		initOTA();
	}

	// Initialize RAK13300 module
	if (init_lorawan() != 0)
	{
		myLog_e("Failed to initialize RAK13300");
	}

	// Initialize BLE interface
	init_ble();
}

/**
 * @brief Arduino loop
 * 
 */
void loop()
{
	// Handle OTA updates
	ArduinoOTA.handle();

	if (g_ota_running)
	{
		return;
	}

	if (!WiFi.isConnected())
	{
		wifi_multi.run();
	}

	// wait for WiFi connection
	if (WiFi.isConnected() && g_lpwan_has_joined)
	{
		digitalWrite(LED_GREEN, HIGH);
		// Get Power and Today's Energy values from Sunnyboy
		String keys[2] = {KEY_POWER, KEY_ENERGY_TODAY};
		int values[2];
		bool isSuccess = smaReader.getValues(2, keys, values);
		myLog_d("Getting values: %s", isSuccess ? "success" : "fail");
		if (isSuccess)
		{
			// at night the current value turns to -1
			if (values[0] == -1)
			{
				values[0] = 0;
			}
			myLog_d("Current power %d W - Collected today %d Wh", values[0], values[1]);
			uint8_t data[6];
			data[0] = 0x20; // Flag for solar system data
			data[1] = values[0] >> 8;
			data[2] = values[0];
			data[3] = values[1] >> 8;
			data[4] = values[1];

			lmh_error_status result = send_lora_packet((uint8_t *)data, 5);
			switch (result)
			{
			case LMH_SUCCESS:
				myLog_d("Packet enqueued");
				break;
			case LMH_BUSY:
				myLog_e("LoRa transceiver is busy");
				break;
			case LMH_ERROR:
				myLog_e("Packet error, too big to send with current DR");
				break;
			}

			/** Buffer for Json object */
			DynamicJsonDocument jsonBuffer(512);

			// Prepare json object for the UDP broadcast
			jsonBuffer["de"] = "spm";
			jsonBuffer["s"] = values[0];
			jsonBuffer["c"] = 0;

			String broadCast;
			serializeJson(jsonBuffer, broadCast);

			// Broadcast the data from the SMA inverter
			udp.broadcastTo(broadCast.c_str(), udpBcPort);
			myLog_d("UDP broadcast done");
		}
		else
		{
			myLog_e("Failed to read data from SMA inverter");
		}

		digitalWrite(LED_GREEN, LOW);

		time_t start_delay = millis();
		while ((millis() - start_delay) < g_lorawan_settings.send_repeat_time)
		{
			// Handle OTA updates
			ArduinoOTA.handle();
			// if (g_ota_running)
			// {
			// 	return;
			// }
			delay(500);
		}
	}
	else
	{
		if (!WiFi.isConnected())
		{
			myLog_d("WiFi not connected");
		}
		delay(5000);
	}
}
