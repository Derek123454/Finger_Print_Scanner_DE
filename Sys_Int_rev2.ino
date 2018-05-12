
#include "LiquidCrystal.h" //for the screen
#include "FPS_GT511C3.h" //the fps (fingerprint scanner) library
#include "SoftwareSerial.h" //used by fps library
#include "SparkFunDS1307RTC.h"
#include "wire.h"
#include "SPI.h"
//#include "SD.h"
#include "PetitFS.h"

#define SD_MAX_OPEN_ATTEMPTS 25
//Setting up the pins for the LCD and the fps
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //pinouts for LCD
FPS_GT511C3 fps(A2, A3); //RX, TX


boolean isFinger = false; //true if the fps detects a finger on the scanner

//output pins

const String idNames[]  =
{
  "self", "Derek", "Ryan", "Mom", "Laura", "Auntie", "Grandma", "Zeide", "person", "person", "Thumb"
};

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting Program"));
  Serial.println(F("Derek Fronek "));
  Serial.println(F("Finger Print Scanner rev. 1 April 2018"));


  //for debugging
  //Serial.begin(9600);
  fps.UseSerialDebug = false; //set to true for fps debugging through serial
  display_freeram();



  fps.Open();
  fps.SetLED(true); //the fps LED
  Serial.println("FPS Started");

  lcd.begin(16, 2);
  rtc.begin();
  Serial.println(F("Clock Started"));
  Serial.println("LCD Started");

  //print starting message
  lcd.print("Sign in"); //the command to print to the LCD
  lcd.setCursor(0, 1); //sets the cursor to the 0th column in the 1st row
  lcd.print(" CCS Schools ");

  delay(3000);


}

void loop() {
  Serial.println("LOOP");
  waitForFinger();


  lcd.clear(); //clears the screen and sets the cursor to 0,0
  fps.CaptureFinger(false); //captures the finger for identification
  int id = fps.Identify1_N(); //identifies print and stores the id

  if (id <= 10) {
    lcd.print(" Now Signed In "); //success message
    lcd.setCursor(0, 1);

    //prints name when the student is logged in
    int minit = rtc.minute();
    String sval = "";
    if (minit < 10) {
      sval += "0" + (char)minit;
    } else
      sval += String(minit);
    String message = "Logged in " + idNames[id] + "," + rtc.hour() + ":" + sval + "," + rtc.date() + "/" + rtc.month() + ",  "  ;
    String m1 = idNames[id] + "!";
    Serial.println(message);
    lcd.print(m1);
    SD_WriteHighScore(0, 0, message.length(), message.c_str());
    delay(1500);





  }


  else {
    lcd.print("Fingerprint is"); //if print isn't recognized
    lcd.setCursor(0, 1);
    lcd.print("   unverified   ");
    delay(2000);
    lcd.clear();
    lcd.print("Please try again");
    lcd.setCursor(0, 1);
    delay(500);
  }
  delay(250);
}




void waitForFinger() {

  static int timer; //contains timeout counter
  timer = 0; //resets the timer everytime this function starts
  while (!fps.IsPressFinger()) { //timeout of eight seconds
    timer++;
    ShowTime();
    delay(100);
    if (timer >= 80 && !fps.IsPressFinger()) {

    }
  }
  timer = 0; //resets the timer everytime this function ends
}

String centerText(String s) { //centers text on the LCD to look better
  while (16 - s.length() > 1) { //if the text needs to be centered
    s = " " + s + " "; //creates space on both sides evenly
  }
  return s;
}

void ShowTime()
{

  rtc.update();
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Time: ");

  lcd.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    lcd.print('0'); // Print leading '0' for minute
  lcd.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    lcd.print('0'); // Print leading '0' for second
  lcd.print(String(rtc.second()));
  lcd.setCursor(0, 1);

  lcd.print("Date: ");

#ifdef PRINT_USA_DATE
  lcd.print(String(rtc.month()) + "/" +   // Print month
            String(rtc.date()) + "/");  // Print date
#else
  lcd.print(String(rtc.month()) + "/" +  String(rtc.date()) + "/"); // Print month
#endif
  lcd.println(String(rtc.year()) + "    ");
}

FATFS fs;
uint8_t SD_OpenFile() {

  FRESULT res;

  for (uint8_t i = 0; i < SD_MAX_OPEN_ATTEMPTS; i++) {

    pf_mount(NULL);
    res = pf_mount(&fs);
    res |= pf_open("TEST.TXT");

    if (!res)
      break;

    if (i == SD_MAX_OPEN_ATTEMPTS - 1)
      return 1;//cannot open file, doesn't exist?
    delay(4);
  }
  return 0;
}




uint8_t SD_WriteHighScore(uint32_t block, uint32_t offset, uint32_t len, void *data) { //user must first pf_mount(), must pass 512 byte buffer

  //  if(block < 8)
  //return 1;//block is reserved for kernel use
  FRESULT res = 0;
  WORD  bt;
  int id = fps.Identify1_N();


  if (SD_OpenFile())
    return 1;//couldn't open file

  pf_lseek((uint32_t)(block * 512UL)); //go to the first block byte

  if (offset || offset + len != 512UL) { //copy the existing data so we don't corrupt it
    if (offset)
      res |= pf_read((BYTE *)data, offset, &bt); //read existing inset bytes
    if (offset + len < 512UL)
      res |= pf_read((BYTE *)data + (offset + len), (512UL - (offset + len)), &bt); //read existing bytes that will follow our data
  }//else overwrite entire block
  display_freeram();
  res |=  pf_write((BYTE *)data, 512UL, &bt); //write the aggregated score data
  Serial.println(res);
  Serial.println((char*)data);
  pf_write(0, 0, 0); //finalize write operation

  if (res || bt != 512UL)
    return 2;//something strange happened, write past file boundary?

  return 0;//write success
}



void display_freeram()
{
  Serial.print(F("-SRAM left="));
  Serial.println(freeRam());
}
int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0
                    ? (int)&__heap_start : (int) __brkval);
}
