// Source 1
// http://forum.arduino.cc/index.php?topic=88388.msg798351#msg798351

		
		#include <SoftwareSerial.h>
		
		// TX_PIN is not used by the sensor, since that the it only transmits!
		#define PING_RX_PIN 6
		#define PING_TX_PIN 7
		
		SoftwareSerial mySerial(PING_RX_PIN, PING_TX_PIN);
		
		long inches = 0, mili = 0;
		byte mybuffer[4] = {0};
		byte bitpos = 0;
		
		void setup() {
		  Serial.begin(9600);
		
		  mySerial.begin(9600);
		}
		
		
		void loop() {
		  bitpos = 0;
		  while (mySerial.available()) {
		    // the first byte is ALWAYS 0xFF and I'm not using the checksum (last byte)
		    // if your print the mySerial.read() data as HEX until it is not available, you will get several measures for the distance (FF-XX-XX-XX-FF-YY-YY-YY-FF-...). I think that is some kind of internal buffer, so I'm only considering the first 4 bytes in the sequence (which I hope that are the most recent! :D )
		    if (bitpos < 4) {
		      mybuffer[bitpos++] = mySerial.read();
		    } else break;
		  }
		  mySerial.flush(); // discard older values in the next read
		
		  mili = mybuffer[1]<<8 | mybuffer[2]; // 0x-- : 0xb3b2 : 0xb1b0 : 0x--
		  inches = 0.0393700787 * mili;
		  Serial.print("PING: ");
		  Serial.print(inches);
		  Serial.print("in, ");
		  Serial.print(mili);
		  Serial.print("mili");
		  
		  delay(100);
		}


// SOURCE 2
// 		
		
		SoftwareSerial mySerial =  SoftwareSerial(D6,D7);
		
		
		
		unsigned int reading;
		byte readByte;
		byte read_buffer[4];
		byte crcCalc;
		word distance;
		String outText;
		
		void setup() {
		  //
		  // Настройка портов
		  //
		  mySerial.begin (9600);
		
		  Serial.begin (115200);
		  Serial.println("start");
		  //
		  // Puffer leeren
		  //
		  for (byte loopstep = 0; loopstep <= 3; loopstep++) {
		    read_buffer[loopstep] = 0;
		  }
		}
		
		void loop() {
		  //
		  // Prüfen Sie, ob Daten in COM-Port
		  //
		  if (mySerial.available() < 1) {
		    return;
		  }
		  //
		  //  In dem Puffer lesen
		  //
		  readByte = mySerial.read();
		
		  for (byte loopstep = 0; loopstep <= 2; loopstep++) {
		    read_buffer[loopstep] = read_buffer[loopstep + 01];
		  }
		
		  read_buffer[03] = readByte;
		  //
		  // Pufferanalyse
		  //
		  if (read_buffer[00] != 0xff) {
		    return; // dies nicht der Anfang der Daten
		  };
		
		  crcCalc = read_buffer[00] + read_buffer[01] + read_buffer[02];
		  if (read_buffer[03] != crcCalc) {
		    return; // Prüfsumme Datenpaket nicht übereinstimmt
		
		  };
		  //
		  // Berechnung Abstand
		
		  //
		  distance = (read_buffer[01] * 0xff) + read_buffer[02];
		  //
		  // print
		  //
		  outText = "bytes: ";
		  outText = String(outText + read_buffer[00]);
		  outText = String(outText + "+");
		  outText = String(outText + read_buffer[01]);
		  outText = String(outText + "+");
		  outText = String(outText + read_buffer[02]);
		  outText = String(outText + "+");
		  outText = String(outText + read_buffer[03]);
		  outText = String(outText + " = ");
		  outText = String(outText + distance);
		  outText = String(outText + " mm");
		
		  Serial.println(outText);
		  //
		  // Delay
		  //
		  delay(1000);
		  //
		  // Aus den Daten entfernen Puffer, dass während der Pause zu geklettert
		
		  //
		  while (mySerial.available() > 0) {
		     readByte = mySerial.read();
		  }
		}
			
