/**
 * MFRC522 Pinout:
 * SDA  - to PIN 8
 * SCK  - to PIN 13 SPI SCK
 * MOSI - to PIN 11 SPI MOSI
 * MISO - to PIN 12 SPI MISO
 * IRQ  - N/C
 * GND  - GND
 * RST  - to PIN 9
 * 3V3  - to 3V3
 * 
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet2.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          8          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
EthernetClient client;
IPAddress ip(192, 168, 0, 223);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
String rfidUid = "", type = "in";
uint32_t byteCount = 0;
bool printWebData = true;
uint32_t times = 0;


void setup() {
	Serial.begin(115200);
	while (!Serial);
	SPI.begin();
  // digitalWrite(10,1);
  // digitalWrite(9,0);

	mfrc522.PCD_Init();		// Init MFRC522
	delay(50);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details

	Serial.println(F("RC Ready, time "));
	Serial.println(millis());
	Serial.println(F("ms from start, init ethernet"));

/*
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
*/
    Ethernet.begin(mac, ip);
    Serial.println(F("ethernet ready, time "));
    Serial.println(millis());
}
void loop() {

  if(mfrc522.PICC_IsNewCardPresent()){
    if(mfrc522.PICC_ReadCardSerial()){
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        rfidUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        rfidUid += String(mfrc522.uid.uidByte[i], HEX);
        mfrc522.PICC_HaltA();
      }
      Serial.println(rfidUid);
    }
  }

  if (rfidUid != ""){
    if (client.connect("192.168.0.224", 80)) { // YOUR SERVER ADDRESS
    Serial.println("connected");
    rfidUid = "type=" + type + "&card=" + rfidUid;
    
    Serial.println(rfidUid);
    
      client.println("POST /woodart/makeEvent.php HTTP/1.1");
      client.println("Host: 192.168.0.224");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(rfidUid.length());
      client.println();
      client.print(rfidUid);
      client.println("Connection: close");
      client.println();
    } else {
      Serial.println("Couldn't connect");
    }

    if (client.connected()) { 
      client.stop();
    }

    rfidUid = "";
  }

}