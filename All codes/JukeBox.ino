#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define SS_PIN 6
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN);    //创建MFRC522实例
MFRC522::MIFARE_Key key; 

int num;

byte nuidPICC[4];
byte IC_Card[][4]={
  {0xF3,0x94,0xD9,0x0B},
  {0x23,0xC9,0x50,0x90},
  {0xEC,0x58,0x65,0x34},
  {0x3C,0x1B,0x6B,0x34},  
  {0x1C,0x27,0x79,0x34},  
  {0xBC,0x6B,0x70,0x34},  
  }; 

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup() { 
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  SPI.begin();            //初始化 SPI 总线
  rfid.PCD_Init();        //初始化 MFRC522 卡
 
 num=sizeof(IC_Card) /sizeof(IC_Card[0]);
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  

  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
}
 
void loop() {

  
  if(!rfid.PICC_IsNewCardPresent())
    return;
 
 
  if(!rfid.PICC_ReadCardSerial())
    return;
 
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  
  Serial.print(F("Dev:"));
  printHex(rfid.uid.uidByte, rfid.uid.size);
 
 for (byte i = 0; i < num; i++) {
    if(IC_Card[i][0]==nuidPICC[0]&&IC_Card[i][1]==nuidPICC[1]&&IC_Card[i][2]==nuidPICC[2]&&IC_Card[i][3]==nuidPICC[3])
    {
        myDFPlayer.play(i+1);
        nuidPICC[0]=0;
        nuidPICC[1]=0;
        nuidPICC[2]=0;
        nuidPICC[3]=0;
    }
  }
  //暂停PICC
  rfid.PICC_HaltA();
  //暂停PCD上的加密
  rfid.PCD_StopCrypto1();
}
 
 
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
  Serial.print("#");
}