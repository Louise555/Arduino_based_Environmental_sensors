
/*
 * 12/29/19  Veronique Lankar - Yelda Balkir - Manhattan College/
 * The mq131 sensor was bought from the manufacturer Winsen. The datasheet can be found here:https://github.com/ostaquet/Arduino-MQ131-driver/tree/master/datasheet
 * The connections for the sensors are explained in the datasheet. The code works for both the low and high concentration sensor
 * We have a load resistance RL of 10K (connected to pins 1,3 of the sensor (see schematic for the connections)
 * The code measures and records the voltage across RL and compute Rs, the resistance of the sensor. The concentration of ozone depends on Rs.
 * The calinration is not done by the code is done in a spreadhsset (see attachment)
 * So the code only focuses on recording Rs. The calibration is based on the sensitive curves deduced from the datasheet. The sensitivity cuves can be found here:
 * https://github.com/ostaquet/Arduino-MQ131-driver/tree/master/datasheet
 * To tune the calibration, one can use a professional sensor monitor or the website:  https://breezometer.com (not as precise)
 * 
 * The sensor neews to be heated by a constant, regulated 5V power source. The low concentration sensor is easily fried. 
 * 
 * We have extended the circuitry with a datta-logger by adafruit. https://learn.adafruit.com/adafruit-data-logger-shield/wiring-and-config
 * The data-logger includes a SD card and a Real Time Clock powered by a 3V battery. It keeps track of time hour and minutes. 
 * To reset the time uncomment :   rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); Once the time is set, comment the line.
 * The data-logger uses the pins A4 and A5 for the clock (Serial bus I2C used). The pins 10,11,12,13 areused by the SD card to record the measurements in a file (Serial bus SPI)
*  we can not use the pins for other tasks.
 * 
 * We used ditigal pin 3 to signal when the system is recording in the SD card. When it is off, the recording is done.
 * 
 * The steps are:
 * 1) open a new file in the SD card. (source: adafruit) and turn on the LED. record the month and the day.
 * 2) wait for 5 minutes
 * 3) record in the SD card for a number of minutes given by : time_minutes. 
 * Before it works properly, the sensor needs to be on for at least 48 hours (preheat time)when new.
 * 4) close the file. Turn off the LED. 
 * 
 * In the file we write: i,hour,minute,Rs,tmperature
 * in the header of the file, we write the month and the day.
 * 
 * Here we temperature is given by the variable temp (17C by default). In the spreadsheet, it can be easily replaced by another temperature.
 * so the 17 is a place holder. 
 * The low concentration sensor ddidn't work well with a temperature sensor. But a temperature sensor TMP36 can be added with a high concentration sensor.
 * 
 * Before recording the Rs, 16 measurements are taken and averaged out (source: Elliot Williams. Programming AVR). In an effort to remove noise. 
 */
 

#include "SD.h" // header for the sd card
#include <Wire.h> // header for I2C library. used for the clock. The pins A4,A5 are used for the communication. 
#include "RTClib.h" // header for the clock 
RTC_PCF8523 rtc; // This is for the clock. instance of the object clock (see adafruit) 

// For communication between Arduino and the SD card pins 10,11,12,13 are used (for SPI bus). 
const int chipSelect = 10;  // we use pin 10 for chip select

// the logging file that will be recorded in the SD card.
File logfile;

unsigned long time_to_read = 300000; // time to preheat the sensor before starting recording. 5 minutes. 
unsigned long time_start = 0;

float Rs; // resistance of the sensor.
int valueRL = 10000; // resistance of the load. we read the voltage A0 across this load. The resistance is between A0 and Gnd. 

unsigned long time_minutes=10; // numbers of minutes we will record in the SD card

float my_hour; // keeping track of the time with the RTC
float my_minute; //

byte i = 0;
byte temp = 17; // temperature in celcius. Can be changed later. 

void setup() {
  
  Serial.begin(9600);

  PORTD = PORTD | 0x8 ; // pin 3  is an output
  DDRD = DDRD | 0x8 ; // pin 3 is on (LED)



  // initialize the SD card. display on screen message error. Source: adafruit. 
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to output, even if you don't use it:
  PORTB = PORTB | 0x4; // pin 10 is an output

  // see if the card is present and can be initialized. source adafruit. 
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");



  // create a new file. but we want to make sure not to overwrite a file previously created. Otherwise er create another one. source:adafruit. 
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  if (! logfile) {
    Serial.println("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);
  Wire.begin();

  // trying clock
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // uncomment to calibrate the time with computer .
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime now = rtc.now();
  logfile.print(now.month(), DEC); // we write the month
  logfile.print(',');
  logfile.println(now.day(), DEC); // we write the day
  

}

void loop() {


  if ((millis() - time_start) >= time_to_read) { // wait for 5 minutes
    for (i=0;i<time_minutes;i++) { // run for a few minutes and record in the SD card
    logfile.print(i);
    logfile.print(',');
    DateTime now = rtc.now();
    logfile.print(now.hour(), DEC);
    logfile.print(',');
    logfile.print(now.minute(), DEC);
    logfile.print(',');
    initADC(); // start the analog to digital converter 
    Rs = float(oversample16x()); // we take 16 measurements and average out
    Rs = (Rs / 1024.0) * 5.0; // convert the digital number (0-1024) to volts. Arduino has a 10-bits analog to digital converter. 
    Rs = (5.0 / Rs - 1.0) * valueRL; // Compute the resistance of the sensor.
    Serial.println(i);
    Serial.print(Rs); // write Rs
    logfile.print(Rs);
    Serial.print(",");
    logfile.print(",");
    Serial.println(temp); // temperature
    logfile.println(temp);
    // wait for 1 minute
    for (int j = 0; j < 60; j++)
    {
      delay(1000);
    }
    }
     //  digitalWrite(3,LOW);
        DDRD=DDRD&0b11110111; // turn off LED
      
        logfile.close(); // close file 
        Serial.println("done");
        while(1);
  }

}
void initADC(void) {
  ADMUX = ADMUX & 0xF0;                     /* set mux to ADC0 . The reading is on A0. */
  // that means the analog voltage is on A0
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC . So around 5V*/
  ADCSRA |= (1 << ADPS1) | (1 << ADPS2) | 1 << ADPS0; /* ADC clock prescaler /128 */
  ADCSRA |= (1 << ADEN);                                 /* enable ADC */
}

uint16_t oversample16x(void) { // record 16 data and then average them. source: Elliot Williams. AVR programming. 
  uint16_t oversampledValue = 0;
  uint8_t i;
  for (i = 0; i < 16; i++) {
    ADCSRA |= (1 << ADSC);                     /* start ADC conversion */
    loop_until_bit_is_clear(ADCSRA, ADSC);          /* wait until done */
    oversampledValue += ADC;                        /* add them up 16x */
  }
  return (oversampledValue >> 4);          /* divide back down by four */
}
