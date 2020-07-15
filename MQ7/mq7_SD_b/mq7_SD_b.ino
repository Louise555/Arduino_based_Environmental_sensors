// Veronique Lankar
// see mq7_test for comments.
// we clean for 60s then collect data for 90s (so 90 data). then we clean again for next time.
//
// 5/6/2020 testing the sensor for CO. bought at amazon


#include <TimerOne.h>
#include "SD.h" // header for the SD linrary
#include <Wire.h> // header for the I2c protocol
#include "RTClib.h" // for the clock
RTC_PCF8523 rtc; // instance of the clock. See adafruit. 
// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. tis is important. 

// the logging file that will be recorded in the SD card.
File logfile;

   
void setup() 
{
    Serial.begin(9600);
    pinMode(4, OUTPUT);  // this LED is on when it is recording.
  pinMode(9, OUTPUT);   // we use  to control the transistor. 
  // we set the period of the PWM mode for pin 9 and 10. 16 us period means a 62,500 Hz frequency as suggested in the paper
    Timer1.initialize(16); // timer 1 -  units in microseconds for the period . f=62,500Hz T=16us 
  pinMode(A0, INPUT); // for the reading

      // initialize the SD card. diaply on screen message error. 
   Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to 10
  
  // output, even if you don't use it:
  pinMode(10, OUTPUT); // this is for I2C communication for the SD card
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  // create a new file. but we want to make sure not to overwrite a file previously created. Otherwise er create another one. up to 99 logfiles
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
  

 // Serial.print("Logging to: "); // uncomment if you use the monitor. 
  //Serial.println(filename);
  Wire.begin();
}

void loop()
{
     digitalWrite(4, HIGH); // turn on the light so we know it is recording.


      Serial.println(" clearing the sensor of gases");
  // Apply 5V for 60 seconds, don't use these readings for CO measurement. 
    Timer1.pwm(9, 1023); // that should be 5V
  for (int i=0;i++;i<60){
  delay(1000);
  }
  // Apply 1.4V for 90 seconds, use these readings for CO measurement.
  int j=0;
  Timer1.pwm(9, 330); // that should be 1.4V
  while(j<90) {
    Serial.println(j);
  Serial.println("reading");
   Serial.println(analogRead(A0));
   logfile.print(j);
   logfile.print(",");
   logfile.println(analogRead(A0));
  delay(999);
  j++;
  }
    // Apply 5V for 60 seconds, don't use these readings for CO measurement. 
    Timer1.pwm(9, 1023); // that should be 5V
  for (int i=0;i++;i<60){
  delay(1000);
  }



      Timer1.pwm(9, 1023); // that should be 5V
  for (int i=0;i++;i<60){
  delay(1000);
  }

   //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    DateTime now = rtc.now(); 
    logfile.print(now.year(), DEC);
    logfile.print(',');
    logfile.print(now.month(), DEC);
    logfile.print(',');
    logfile.print(now.day(), DEC);
    logfile.print(',');
    logfile.print(now.hour(), DEC);
    logfile.print(',');
    logfile.print(now.minute(), DEC);
    delay(1000);
    Serial.println(" done");
    logfile.close();
    digitalWrite(4, LOW);
    while(1) {}
  }
    
 
  
  
