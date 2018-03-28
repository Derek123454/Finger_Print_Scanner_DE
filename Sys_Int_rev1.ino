

#include "LiquidCrystal.h" //for the screen
#include "FPS_GT511C3.h" //the fps (fingerprint scanner) library
#include "SoftwareSerial.h" //used by fps library
#include "SparkFunDS1307RTC.h"
#include "wire.h"
#include "SPI.h"
//#include "SD.h"
//Setting up the pins for the LCD and the fps
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //pinouts for LCD
FPS_GT511C3 fps(8, 9); //RX, TX


boolean isFinger = false; //true if the fps detects a finger on the scanner

//output pins

const String idNames[] = 
{
  "self","Bro", "Ryan", "Mom", "Laura", "Auntie", "Grandma", "Zeide", "person", "person", "Thumb"};

void setup(){
  //Serial.begin(9600);
  //Serial.println("Starting Program");
  rtc.begin();
  //Serial.println("Clock Started");
  //for debugging
  //Serial.begin(9600);
  fps.UseSerialDebug = false; //set to true for fps debugging through serial

  
  lcd.begin(16,2);
   //Serial.println("LCD Started");
  fps.Open();
  fps.SetLED(true); //the fps LED
  //Serial.println("FPS Started");

  //print starting message
  lcd.print("Sign in"); //the command to print to the LCD
  lcd.setCursor(0, 1); //sets the cursor to the 0th column in the 1st row
  lcd.print(" CCS Schools ");
  
  delay(3000);

  
/*Serial.print("Initializing SD card...");

//
// see if the card is present and can be initialized:
if (!SD.begin()) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
Serial.println("card initialized.");
*/
  

}
void loop(){
  
  waitForFinger();
  
  lcd.clear(); //clears the screen and sets the cursor to 0,0
  fps.CaptureFinger(false); //captures the finger for identification
  int id = fps.Identify1_N(); //identifies print and stores the id

  if(id <= 10){
    lcd.print(" Now Signed In "); //success message
    lcd.setCursor(0,1);

    //prints name when the student is logged in
    String message =  idNames[id] + "!";
    lcd.print(message);
    //SDWrite();
    
    delay(1500);

    
    
    

  }


  else{
    lcd.print("Fingerprint is"); //if print isn't recognized
    lcd.setCursor(0,1);
    lcd.print("   unverified   ");
    delay(2000);
    lcd.clear();
    lcd.print("Please try again");
    lcd.setCursor(0,1);
    delay(500);
  }
  delay(250);
}




void waitForFinger(){
  
  static int timer; //contains timeout counter
  timer = 0; //resets the timer everytime this function starts
  while(!fps.IsPressFinger()){ //timeout of eight seconds
    timer++;
    ShowTime();
    delay(100); 
    if (timer>=80 && !fps.IsPressFinger()){
      
    }
  } 
  timer = 0; //resets the timer everytime this function ends
}

String centerText(String s) { //centers text on the LCD to look better
  while(16-s.length()>1){ //if the text needs to be centered
    s = " " + s + " "; //creates space on both sides evenly
  }
  return s;
}

void ShowTime() 
{

 rtc.update();
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print("Time: ");

 lcd.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    lcd.print('0'); // Print leading '0' for minute
  lcd.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    lcd.print('0'); // Print leading '0' for second
  lcd.print(String(rtc.second()));
 lcd.setCursor(0,1);

 lcd.print("Date: ");
    
#ifdef PRINT_USA_DATE
  lcd.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  lcd.print(String(rtc.month()) + "/" +  String(rtc.date()) + "/"); // Print month
#endif
  lcd.println(String(rtc.year()));
}
//File myFile;
/*void SDWrite() {
    

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("log.csv", FILE_WRITE);
  int id = fps.Identify1_N();
  String message = "  Logged in " + idNames[id] + "," + rtc.hour() + ":" + rtc.minute() + "," + rtc.date() + "/" + rtc.month() + ","  ;
  // if the file opened okay, write to it:
  if (myFile) {
   
    myFile.print(message);
    // close the file:
    myFile.close();
    
  }

 
 
}*/




