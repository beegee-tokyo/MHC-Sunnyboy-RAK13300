/**
 * @file app_ota.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief OTA callbacks for ESP32 OTA DFU
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

/** Status of OTA */
bool g_ota_running = false;

/**
 * @brief Initialize OTA and provide the callbacks
 * 
 */
void initOTA()
{
	char hostApName[] = "MHC-SPM-xxxxxxxx";
	// Create device ID from MAC address
	String macAddress = WiFi.macAddress();
	memcpy((void *)&hostApName[8], (void *)&macAddress[0], 2);
	memcpy((void *)&hostApName[10], (void *)&macAddress[9], 2);
	memcpy((void *)&hostApName[12], (void *)&macAddress[12], 2);
	memcpy((void *)&hostApName[14], (void *)&macAddress[15], 2);

	// Prepare OTA update listener
	ArduinoOTA.onStart([]()
					   {
						   String debugMsg = "OTA start";
						   Serial.println(debugMsg);
						   g_ota_running = true;
						   digitalWrite(LED_BLUE, HIGH); // Turn on blue LED
						   digitalWrite(LED_GREEN, LOW); // Turn on green LED
					   });
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
						  {
							  digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));	// Toggle blue LED
							  digitalWrite(LED_GREEN, !digitalRead(LED_GREEN)); // Toggle green LED
						  });
	ArduinoOTA.onError([](ota_error_t error)
					   {
						   digitalWrite(LED_BLUE, HIGH);  // Turn on blue LED
						   digitalWrite(LED_GREEN, HIGH); // Turn on green LED
					   });
	ArduinoOTA.onEnd([]()
					 {
						 digitalWrite(LED_BLUE, LOW);	// Turn off blue LED
						 digitalWrite(LED_GREEN, HIGH); // Turn off green LED
					 });

	// Start OTA server.
	ArduinoOTA.setHostname(hostApName);
	ArduinoOTA.begin();

	MDNS.addServiceTxt("arduino", "tcp", "board", "ESP32");
	MDNS.addServiceTxt("arduino", "tcp", "type", "Solar Panel Monitor");
	MDNS.addServiceTxt("arduino", "tcp", "id", "MHC-SPM");
	MDNS.addServiceTxt("arduino", "tcp", "service", "MHC");
	MDNS.addServiceTxt("arduino", "tcp", "loc", "Frontyard");
}
