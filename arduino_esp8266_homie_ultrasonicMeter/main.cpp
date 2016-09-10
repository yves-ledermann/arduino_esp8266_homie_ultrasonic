// Do not remove the include below
#include "main.h"

/*
 * esp8266_homie_sonoff.cpp
 *
 *  Created on: 14.08.2016
 *      Author: Yves Ledermann
 *      Based on the work of: 	- https://github.com/marvinroger/homie-esp8266/issues/112
 *      						- https://github.com/unaiur/homie-nodes/tree/master/examples/ledstrip/src
 */

// HW-Config
const int PIN_SERIAL_RX = D8;

//Homie Nodes
HomieNode usonicNode("usonic", "distance");

// Software Serial Port
SoftwareSerial mySerial = SoftwareSerial(PIN_SERIAL_RX, NULL);

unsigned int reading;
unsigned long lastLoop8000ms = 0;


void serialFlush(){
  while(mySerial.available() > 0) {
    char t = mySerial.read();
  }
}


int getdistance() {

	String outText;
	int distance = 0;
	//byte readByte;
	//byte read_buffer[4];
	byte crcCalc;
	byte mybuffer[4] = { 0 };
	byte bitpos = 0;

	//
	// Prüfen ob Daten in Port-Puffer
	//
	if (mySerial.available() < 1) {
		Serial.println("keine Daten im Puffer");
		return -1;
	}

	//
	//  Aus dem Puffer lesen
	//
	while (mySerial.available()) {
		// the first byte is ALWAYS 0xFF and I'm not using the checksum (last byte)
		// if your print the mySerial.read() data as HEX until it is not available, you will get several measures for the distance (FF-XX-XX-XX-FF-YY-YY-YY-FF-...). I think that is some kind of internal buffer, so I'm only considering the first 4 bytes in the sequence (which I hope that are the most recent! :D )
		if (bitpos < 4) {
			mybuffer[bitpos++] = mySerial.read();
		} else
			break;
	}

	Serial.print("Buffer[0]: ");
	Serial.println(mybuffer[0],DEC);
	Serial.print("Buffer[1]: ");
			Serial.println(mybuffer[1],DEC);
	Serial.print("Buffer[2]: ");
			Serial.println(mybuffer[2],DEC);
	Serial.print("Buffer[3]: ");
			Serial.println(mybuffer[3],DEC);


	// flush the receive buffer
	serialFlush();

	//
	// Pufferanalyse
	//
	if (mybuffer[00] != 0xff) {
		Serial.println("nicht der Anfang der Daten");
		return -2; // dies nicht der Anfang der Daten
	};

	crcCalc = mybuffer[00] + mybuffer[01] + mybuffer[02];
	if (mybuffer[03] != crcCalc) {
		Serial.println("CRC Error");
		return -3; // Prüfsumme Datenpaket nicht übereinstimmt

	};
	//
	// Berechnung Abstand

	//
	distance = (mybuffer[01] * 0xff) + mybuffer[02];
	//
	// print
	//
	outText = "bytes: ";
	outText = String(outText + mybuffer[00]);
	outText = String(outText + "+");
	outText = String(outText + mybuffer[01]);
	outText = String(outText + "+");
	outText = String(outText + mybuffer[02]);
	outText = String(outText + "+");
	outText = String(outText + mybuffer[03]);
	outText = String(outText + " = ");
	outText = String(outText + distance);
	outText = String(outText + " mm");
	outText = String(outText + " CRC-SUM ");
	outText = String(outText + crcCalc);


	Serial.println(outText);

	return distance;
}

void loopHandler() {
	int distance = 0;
	int olddistance = 0;

	if (millis() - lastLoop8000ms >= 2000UL || lastLoop8000ms == 0) {
		Serial.println("LOOP 8000ms");

		distance = getdistance();

		Serial.print("Rückgabewert: ");
		Serial.println(distance);

		if (distance != olddistance) {

			 if (Homie.setNodeProperty(usonicNode, "mm", String(distance), true)) {
				 Serial.println("SETNODE ok");
			    } else {
			      Serial.println("SETNODE error");
			    }

			olddistance = distance;
		}

		lastLoop8000ms = millis();

	}
}

void setup() {
	Serial.begin(115200);
	Serial.println();
	Serial.println();

	Homie.disableResetTrigger();
	//Homie.disableLedFeedback();
	Homie.enableBuiltInLedIndicator(false);
	mySerial.begin(9600);

	//
	// Puffer leeren
	//
	// flush the receive buffer
	serialFlush();


	//Homie.__setFirmware("esp8266_homie_usonic", "20160907");
	//Homie.setLedPin(PIN_LED, LOW);
	//Homie.setResetTrigger(PIN_BUTTON, LOW, 10000);
	//switchNode.subscribe("on", switchOnHandler);
	Homie.registerNode(usonicNode);
	Homie.setLoopFunction(loopHandler);
	Homie.setup();
}

void loop() {
	Homie.loop();
	//handleOTA();
}
