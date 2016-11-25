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
  int stat = 1;
  if ( findID(readCard) ) {  // If not, see if the card is in the EEPROM
    Serial.println(F("Welcome, You shall pass"));
    stat=0;
  }
  else {      // If not, show that the ID was not valid
    Serial.println(F("You shall not pass"));
    stat = 1;
  }
  if(stat==1){
    lcd.print("Access Denied");
    redSolid();
  }
  else if(stat==0){
    lcd.print("Access Accept");
    granted(3000);// Open the door lock for 300 ms
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

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
boolean findID( byte find[] ) {
  int count = EEPROM.read(0);      // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;  // Stop looking we found it
    }
    else {    // If not, return false
    }
  }
  return false;
}

//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void readID( int number ) {
  int start = (number * 4 ) + 2;     // Figure out starting position
  for ( int i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Find Slot   ///////////////////////////////////
int findIDSLOT( byte find[] ) {
  int count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
      break;          // Stop looking we found it
    }
  }
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void writeID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( 0, num );     // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
    successWrite();
  Serial.println(F("Succesfully added ID record to EEPROM"));
  }
  else {
    failedWrite();
  Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}
