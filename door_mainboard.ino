#include <SPI.h>        // RC522 Module uses SPI protocol
#include <SD.h>         // We are going to read and write PICC's UIDs from/to SD
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM

byte readCard[4];    // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID
byte storedCard[4];
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

boolean match = false;          // initialize card match to false
boolean programMode = false;  // initialize programming mode to false
int successRead;
void setup() {
  Serial.begin(9600);
  successRead = 0;
  ledsetup();
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop() {
    do {
    // sets successRead to 1 when we get read from reader otherwise 0
    if (programMode) {
      cycleLeds();              // Program Mode cycles through RGB waiting to read a new card
    }
    else {
      normalModeOn();     // Normal mode, blue Power LED is on, all others are off
    }
  }
  while (!successRead);   //the program will not go further while you not get a successful read
  int stat = checkID();
  if(stat==1){
    lcd.print("Access Denied");
    redSolid();
  }
  else if(stat==0){
    lcd.print("Access Accept");
    granted(100);
  }

  if(successRead){
    Serial.println("reset");
    lcd.clear();
    lcd.setCursor(0, 0);
    successRead=0;
  }
  
}


void readUIDCard(){
   Serial.println(F("Scanned PICC's UID: "));
   for (int i = 0; i < 4; i++) {  //
     Serial.print(readCard[i],HEX);
   }
   Serial.println();
}

int checkID(){
  return 1;
}

/////////////////////////////////// Recive from I2C ///////////////////////////////////

void receiveEvent(int howMany){
  int i = 0;
  while(Wire.available()){
    byte a = Wire.read();
    readCard[i] = a;
    i++;
  }
  successRead = 1;
  readUIDCard();
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL )       // Make sure there is something in the array first
    match = true;       // Assume they match at first
  for ( int k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] )     // IF a != b then set match = false, one fails, all fail
      match = false;  
  }
  if ( match ) {      // Check to see if if match is still true
    return true;      // Return true
  }
  else  {
    return false;       // Return false
  }
}

boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}
