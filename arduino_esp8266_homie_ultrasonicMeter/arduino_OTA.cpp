/*
 * arduino_OTA.cpp
 *
 *  Created on: 14.08.2016
 *      Author: yves
 *      Source: https://github.com/unaiur/homie-nodes/blob/master/examples/ledstrip/src/ota.cpp
 */
#include "arduino_OTA.h"

void handleOTA() {
	static bool otaEnabled = false;
	if (!WiFi.isConnected())
		return;

	if (otaEnabled) {
		ArduinoOTA.handle();
		return;
	}

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname(WiFi.hostname().c_str());

	// No authentication by default
	// ArduinoOTA.setPassword((const char *)"123");

	ArduinoOTA.onStart([]() {
		Serial.println("Start");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
	otaEnabled = true;
	Serial.println(F("ArduinoOTA enabled."));
}
