#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5  
#define RST_PIN  0  
 
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Approach your reader card...");

    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.print("Card UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    Serial.print("Card SAK: ");
    Serial.println(rfid.uid.sak, HEX);

    Serial.print("PICC type: ");
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    Serial.println("Please choose an option:");
    Serial.println("0: Read data from the card");
    Serial.println("1: Write data to the card");

    while (!Serial.available());  // Wait for user input
    char option = Serial.read();
    
    if (option == '0') {
        Serial.println("Reading data...");
        byte buffer[18];
        byte size = sizeof(buffer);
        MFRC522::StatusCode status = rfid.MIFARE_Read(4, buffer, &size);

        if (status == MFRC522::STATUS_OK) {
            Serial.print("Read Data: ");
            for (byte i = 0; i < 16; i++) {
                Serial.print(buffer[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
        } else {
            Serial.println("Authentication failed: Timeout in communication.");
        }
    } else if (option == '1') {
        Serial.println("Writing data to the card...");
        byte dataBlock[16] = {'H', 'e', 'l', 'l', 'o', ' ', 'R', 'F', 'I', 'D', '!', 0, 0, 0, 0, 0};
        MFRC522::StatusCode status = rfid.MIFARE_Write(4, dataBlock, 16);

        if (status == MFRC522::STATUS_OK) {
            Serial.println("Write successful.");
        } else {
            Serial.println("Authentication failed: Timeout in communication.");
        }
    } else {
        Serial.println("Invalid option. Try again.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
