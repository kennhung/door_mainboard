#include <SPI.h>        // RC522 Module uses SPI protocol
#include <SD.h>         // We are going to read and write PICC's UIDs from/to SD
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

File myFile;
byte readCard[4];    // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID
char dir[] = "PICCS/";
char masdir[] = "master.txt";
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

File masterfile;
int successRead;
void setup() {
  Serial.begin(9600);
  successRead = 0;
  lcd.begin(16, 2);
  // 閃爍三次
  for(int i = 0; i < 3; i++) {
    lcd.backlight(); // 開啟背光
    delay(250);
    lcd.noBacklight(); // 關閉背光
    delay(250);
  }
  ledsetup();
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.print("Initializing SD card...");
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    lcd.print("error");
    redSolid();
    return;
  }
  Serial.println("initialization done.");
  
  readMaster();
}

void loop() {
  int stat = checkID();
  if(stat==1){
    redSolid();
  }
  else if(stat==0){
    greenSolid();
  }

  if(successRead){
    Serial.println("read");
    successRead=0;
  }
  else{
    for(int i =0; i<4;i++){
      readCard[i] = 0;
    }
    blueSolid();
  }
  
}

void readMaster(){
  if (SD.exists(masdir)) {
    Serial.println("Master card exists.");
    Serial.print(F("Master Card's UID: "));      // Since we have it print to serial
    masterfile = SD.open(masdir);  // Open file
    for (int i = 0; i < 4; i++) {             // Loop 4 times to get 4 bytes
      readCard[i] = masterfile.read();
      Serial.print(readCard[i],HEX);         // Actual serial printing of each byte
      masterCard[i] = readCard[i];            // Prepare bytes for future comparing
    }
    Serial.println("");
    masterfile.close();
  } else {
    Serial.println("Master card doesn't exist.");
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      blueBlink(); // Visualize Master Card need to be defined
    }
    while (!successRead); //the program will not go further while you not get a successful read
    delay(100);
    Serial.println("Creating Master card");
    masterfile = SD.open(masdir,FILE_WRITE);
    if(masterfile){
      Serial.println("Writing to master.txt...");
      masterfile.write(readCard, 4); 
      for (int i = 0; i < 4; i++) {             // Loop 4 times to get 4 bytes
        readCard[i] = masterfile.read();
        masterCard[i] = readCard[i];            // Prepare bytes for future comparing
      }
      // close the file:
      masterfile.close();
      Serial.println(F("Master Card successfuly defined"));
      for (int i = 0; i < 4; i++) {  //
       Serial.print(masterCard[i], HEX);
     }
     greenBlink();
    }
    else{
      Serial.println("creating master file fail!!!");
      redBlink();
    }
  }
}

void readUIDCard(){
     Serial.println(F("Scanned PICC's UID: "));
     for (int i = 0; i < 4; i++) {  //
       Serial.print(readCard[i],HEX);
     }
     Serial.println();
  }

void receiveEvent(int howMany){
  int i = 0;
  while(Wire.available()){
    byte a = Wire.read();
    readCard[i] = a;
    i++;
  }
  successRead = 1;
  readUIDCard();
  checkID();
}

int checkID(){
  if(successRead){
    for(int i = 0; i<4; i++){
      if(readCard[i] == masterCard[i]){
        
      }
      else{
        return 1;
      }
    }
    return 0;
  }
  else{
    return 2;
  }
}

