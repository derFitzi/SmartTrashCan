//*******************************************************************************************************
//**  Beispielsketch für das Auslesen von ID-Tags. Bei passender ID wird PIN 13 für 2 Sekunden         **
//**  eingeschaltet (LED). Bei falscher ID wird an Pin 12 ein Ton erzeugt. (PASSIVEN Buzzer / Piezzo)  **
//**  nutzen. Statt einer LED kann auch ein Türschloss geschaltet werden (über TIP120 Schaltverstärker **
//*******************************************************************************************************

#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   4   // ESP32 Pin für IRQ
#define PN532_RESET 16   // ESP32 Pin für RESET

const int AlarmPin  =  12;   // Piezo Summer
const int OutputPin =  13;   // LED oder Türschloss

unsigned long cardid;
unsigned long TAGid1 = 1702536620;
unsigned long TAGid2 = 4070796058;
unsigned long TAGid3;

// I2C Pins ESP32
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET); // IRQ + RESET (I2C Standard intern)

void setup() {
  pinMode(AlarmPin, OUTPUT);
  pinMode(OutputPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("Hallo!");

  Wire.begin(SDA_PIN, SCL_PIN); // ESP32 I2C starten

  nfc.begin();

  unsigned long versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Kann kein Board finden !");
    while (1);
  }

  Serial.print("Chip PN5 gefunden"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();
  Serial.println("Warte auf einen ISO14443A Chip ...");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);

    if (uidLength == 4) {
      cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];
      cardid <<= 8;
      cardid |= uid[3];
      Serial.print("Scheint eine Mifare Classic zu sein #");
      Serial.println(cardid);
      Serial.println("");
      Serial.println("");
    }

    if ((cardid == TAGid1)||(cardid == TAGid2)||(cardid == TAGid3)) {
      digitalWrite(OutputPin,HIGH);
      delay(2000);
      digitalWrite(OutputPin,LOW);
    } else {
      tone(AlarmPin, 1000);
      delay(4000);
      noTone(AlarmPin);
    }
  }
}
