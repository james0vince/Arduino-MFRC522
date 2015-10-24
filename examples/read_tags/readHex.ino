#include <MFRC522.h>
#include <SPI.h>

#define SAD 10
#define RST 5

MFRC522 nfc(SAD, RST);

int count = 0;
char name[0] = "David";

void setup() {
  SPI.begin();
  // Read a fast as possible. There is a limit for how long we are
  // allowed to read from the tags.
  Serial.begin(115200);
  Serial.println("-----------------------------------------------------------------------");
  Serial.println("--------------------------Looking for MFRC522--------------------------");
  nfc.begin();

  // Get the firmware version of the RFID chip
  byte version = nfc.getFirmwareVersion();
  if (! version) {
    Serial.println("------------------------Didn't find MFRC522 board.-----------------------");
    while(1); //halt
  }

  Serial.println("--------------------------Found chip MFRC522---------------------------");
  Serial.print("--------------------------Firmware ver. 0x");
  Serial.print(version, HEX);
  Serial.println("---------------------------");
}

byte keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
byte keyB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };

void loop() {
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  int i, j, pos;

  // Send a general request out into the aether. If there is a tag in
  // the area it will respond and the status will be MI_OK.
  status = nfc.requestTag(MF1_REQIDL, data);

  if (status == MI_OK) {
    Serial.println("-----------------------------------------------------------------------");
    Serial.println("------------------------------NFC Card---------------------------------");
    Serial.println("-------------------------------Reader----------------------------------");
    Serial.println("----------------------------Version-1.0--------------------------------");
    Serial.println("------------------------------By--JaXx---------------------------------");
    Serial.println("-----------------------------------------------------------------------");
    Serial.println("------------------------------Starting---------------------------------");
    Serial.println("-----------------------------------------------------------------------");
    Serial.print("Type: ");
    Serial.print(data[0], HEX);
    Serial.print(", ");
    Serial.println(data[1], HEX);
    Serial.println("-----------------------------------------------------------------------");
    // calculate the anti-collision value for the currently detected
    // tag and write the serial into the data array.
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    Serial.println("-----------------------------------------------------------------------");
    Serial.print("The serial nb of the tag is: ");
    for (i = 0; i < 3; i++) {
      Serial.print(serial[i], HEX);
      Serial.print(", ");
    }
    Serial.println(serial[3], HEX);
    Serial.println("-----------------------------------------------------------------------");
    Serial.println("-----------------------------------------------------------------------");
    Serial.println("Authentication: Success.");
    Serial.print("Welcome ");
    Serial.println(name[0]);
    Serial.println("-----------------------------------------------------------------------");
    // Select the tag that we want to talk to. If we don't do this the
    // chip does not know which tag it should talk if there should be
    // any other tags in the area..
    nfc.selectTag(serial);
    // Assuming that there are only 64 blocks of memory in this chip.
    for (i = 0; i < 5; i++) {
      //Serial.println("-----------------------------------------------------------------------");
      // Try to authenticate each block first with the A key.
      status = nfc.authenticate(MF1_AUTHENT1A, i, keyA, serial);
      if (status == MI_OK) {
        Serial.print("\n");
        //Serial.print("Authenticated block nb. 0x");
        Serial.print("Block 0x");
        Serial.print(i, HEX);
       // Serial.println(" with key A.");
       Serial.print(": ");
      // Try to authenticate each block first with the A key.
        // Reading block i from the tag into data.
        status = nfc.readFromTag(i, data);
        if (status == MI_OK) {
          // If there was no error when reading; print all the hex
          // values in the data.
          // Change 64 to mean how many bits you want to read
          for (j = 0; j < 16; j++) {
            Serial.print(data[j], HEX);
            Serial.print(", ");
          }
        } else {
          Serial.println("\n");
          Serial.println("-----------------------------------------------------------------------");
          Serial.println("Read failed.");
          Serial.println("-----------------------------------------------------------------------");
        }
      } else {
         Serial.println("Failed to read this section of the chip.");
      }
    }
    // Stop the tag and get ready for reading a new tag.
    nfc.haltTag();
    Serial.println("\n");
    Serial.println("-----------------------------------------------------------------------");
    Serial.println("\n");
    Serial.println("\n");
  }
  delay(3000);
}
