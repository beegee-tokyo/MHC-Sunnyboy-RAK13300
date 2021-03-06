/**
 * Example: SMAReader_Demo.ino
 *
 */
#include "main.h"

/** Address of SMA Sunnyboy Inverter */
IPAddress inverterIP(192, 168, 1, 127);

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

bool isSuccess = false;

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

	init_display();

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
		int values[2] = {0, 0};
		isSuccess = false;
		int retry_count = 0;
		while (!isSuccess)
		{
			myLog_d("Heap: %ld", ESP.getFreeHeap());
			myLog_d("Stack: %ld", uxTaskGetStackHighWaterMark(NULL));
			isSuccess = smaReader.getValues(2, keys, values);
			myLog_d("Getting values: %s", isSuccess ? "success" : "fail");
			BLE_PRINTF("Getting values: %s", isSuccess ? "success" : "fail");
			if (isSuccess && (values[0] < 3000))
			{
				// at night the current value turns to -1
				if (values[0] == -1)
				{
					values[0] = 0;
				}

				write_display(values[0], values[1]);

				myLog_d("Current power %d W - Collected today %d Wh", values[0], values[1]);
				BLE_PRINTF("Current power %d W - Collected today %d Wh", values[0], values[1]);
				uint8_t data[6];
				data[0] = 0x20; // Flag for solar system data
				data[1] = values[0] >> 8;
				data[2] = values[0];
				data[3] = values[1] >> 8;
				data[4] = values[1];

				// uint8_t lpp_data[16];
				// lpp_data[0] = 0x01; // channel 1
				// lpp_data[1] = 0x02; // analog data
				// value_s solar_data;
				// solar_data.val_16 = values[0]/10;
				// lpp_data[2] = solar_data.val_8[1];
				// lpp_data[3] = solar_data.val_8[0];
				// lpp_data[4] = 0x02; // channel 1
				// lpp_data[5] = 0x02; // analog data
				// solar_data.val_16 = values[1] / 10;
				// lpp_data[6] = solar_data.val_8[1];
				// lpp_data[7] = solar_data.val_8[0];

				lmh_error_status result = send_lora_packet((uint8_t *)data, 5);
				// lmh_error_status result = send_lora_packet(lpp_data, 8);
				switch (result)
				{
				case LMH_SUCCESS:
					myLog_d("Packet enqueued");
					BLE_PRINTF("Packet enqueued");
					break;
				case LMH_BUSY:
					myLog_e("LoRa transceiver is busy");
					BLE_PRINTF("LoRa transceiver is busy");
					break;
				case LMH_ERROR:
					myLog_e("Packet error, too big to send with current DR");
					BLE_PRINTF("Packet error, too big to send with current DR");
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

				// decouple LoRa and WiFi transmission
				delay(5000);
				// Broadcast the data from the SMA inverter
				udp.broadcastTo(broadCast.c_str(), udpBcPort);
				myLog_d("UDP broadcast done");
				BLE_PRINTF("UDP broadcast done");
			}
			else if (values[0] >= 3000)
			{
				myLog_e("Values not valid");
				BLE_PRINTF("Values not valid");
			}
			else
			{
				retry_count++;
				myLog_e("Failed to read data from SMA inverter");
				BLE_PRINTF("Failed to read data from SMA inverter");
				if (retry_count == 5)
				{
					break;
				}
				time_t start_delay = millis();
				while ((millis() - start_delay) < 5000)
				{
					// Handle OTA updates
					ArduinoOTA.handle();
					// if (g_ota_running)
					// {
					// 	return;
					// }
					delay(100);
				}
			}
			// Handle OTA updates
			ArduinoOTA.handle();
			if (g_ota_running)
			{
				return;
			}
		}

		if (!isSuccess)
		{
			write_display(values[0], values[1]);
		}

		digitalWrite(LED_GREEN, LOW);

		time_t start_delay = millis();
		while ((millis() - start_delay) < g_lorawan_settings.send_repeat_time)
		{
			// Handle OTA updates
			ArduinoOTA.handle();
			if (g_ota_running)
			{
				return;
			}
			delay(500);
		}
	}
	else
	{
		if (!WiFi.isConnected())
		{
			myLog_d("WiFi not connected");
			BLE_PRINTF("WiFi not connected");
		}
		delay(5000);
	}
}
