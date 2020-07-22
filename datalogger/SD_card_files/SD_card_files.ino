// this code is to test again the SD card and to save files with data recorded in it/
// if the file already exist, it gives another name to the file.
// 
// https://learn.adafruit.com/adafruit-data-logger-shield/using-the-sd-card
// 
// we pretend to have 2 sensors to record temperature and light intensity
// We have no sensor connected so it is just recording static electricity at the pins A0 and A1. There is no connection necessary
// except if you want to. It is supposed to have LED at pins 3 (red) and 4 (green) but it is optional. 

#include "SD.h"
#include <Wire.h>

// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  1000 // mills between entries
#define ECHO_TO_SERIAL   1 // echo data to serial port. if 1 we arite on the monitor. if 0 we don't.  0 means don't write anything on monitor.
#define WAIT_TO_START    0 // Wait for serial input in setup(). if 0 we don't wait for you to enter a character on monitor to start 


// the digital pins that connect to the LEDs
#define redLEDpin 3
#define greenLEDpin 4

// The analog pins that connect to the sensors
#define photocellPin 0           // analog 0. this is a sensor connected to arduino A0
#define tempPin 1                // analog 1. This is another sensor connected to Arduino A1



// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;  // we use pin 10. tis is important. 

// the logging file?
File logfile;

void error(char *str)   // this function is used to write an error when something goes wrong.
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);
  
  while(1);
}
//
//
void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START
//
//
  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  // create a new file. but we want to make sure not to overwrite a file previously created. Otherwise er create another one.
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);
  Wire.begin();  

  

  logfile.println("millis,light,temp");    // we write a header in the SD card

  Serial.println("millis,light,temp");

  
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
 
   // If you want to set the aref to something other than 5v
  //analogReference(EXTERNAL);
  
}
void loop(void)
{
  int i=0;
  while(i<11){
  // DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  digitalWrite(greenLEDpin, HIGH);

  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m);           // milliseconds since start
  logfile.print(", ");    

  Serial.print(m);         // milliseconds since start
  Serial.print(", ");  

int photocellReading = analogRead(photocellPin);  
  delay(10);
  int tempReading = analogRead(tempPin);    
  

     
  logfile.print(photocellReading);
  logfile.print(", ");    
  logfile.println(tempReading);

  Serial.print(photocellReading);
  Serial.print(", ");    
  Serial.println(tempReading);


  digitalWrite(greenLEDpin, LOW);

i=i+1;
}
logfile.close();
while(1) {};
}
