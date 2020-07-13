// Note: if you want to test the module first. All you have to do is to read the voltage at the NO2 pin of the module- Connect the NO2 pin of the module to the A0 pin of arduino.
// 12/1/19 - Veronique Lankar 
//code to read the NO2 level. We use the data-logger from adafruit and we write: month/day then each field: time,hour, Vs (0-1023)
// the module/breakout is easy to use. We read Vs (voltage across the resistance of the sensor) at the pin A0 of Arduino. 
// the module we use includes the sensor and some circuitry (caps and the circuit 5V - RL - Rs - Gnd). Rs is the resistance of the sensor. RL is a load resistance. 
// we don't have to worry about the circuitry. 
// The calibration to turn Vs to Rs is done in a spreadsheet. 
// the breakout was bought here:
//https://www.ebay.com/itm/MICS-6814-Chip-6814-Carbon-Monoxide-Nitrogen-Oxygen-Compact-Sensor-CO-NO2-NH3/173675327867?hash=item286fdb317b:g:84IAAOSwKM1cBsDE
// the datasheet/sensor  is from:
// https://sgx.cdistore.com/820datasheets/sgx/1143_datasheet%20mics-6814%20rev%208.pdf
// 
// The calibration is done in a separate spreadsheet where Vs (read at one pin of the module) it turned to Rs. Then to Rs/R0
// The concentration of NO2 depends on Rs/R0
// according to the datasheet the concentration is 0.15ppm when Rs=R0. 
// 
//  We record one data every minute for an amount of minutes given by the constant time_minutes
//
// 
// we have a LED at digital 3

//data logger is from from adafruit. https://learn.adafruit.com/adafruit-data-logger-shield/wiring-and-config
// so the pins A4 and A5 are for I2C for the clock. The pins 10,11,12,13 are for SPI for the SD card.
// we can not use the pins for other tasks.


// LED on digital 3.
// - Sensor analog read on pin A0

#include "SD.h" // file for the sd card
#include <Wire.h> // header for I2C library. used for the clock
#include "RTClib.h" // header for the clock 
RTC_PCF8523 rtc; // This is for the clock. instance of the object 

// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. For SPI used for the SD card. Its for chip select

// the logging file that will be recorded in the SD card.
File logfile;


float Vs; // resistance of the sensor.
unsigned long time_minutes=10; // numbers of minutes we will record in the SD card


float my_hour; // keeping track of the time.
float my_minute;
float my_second;
byte i = 0;


void setup() {
  Serial.begin(9600);

  PORTD = PORTD | 0x8 ; // pin 3  is an output
  DDRD = DDRD | 0x8 ; // pin 3 is on (LED)



  // initialize the SD card. display on screen message error.
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  // pinMode(10, OUTPUT); // this is for I2C communication for the SD card
  PORTB = PORTB | 0x4;

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
  logfile.print(now.month(), DEC);
  logfile.print(',');
  logfile.println(now.day(), DEC);
 
   initADC(); // analog to digital converter
}

void loop() {



    for (i=0;i<time_minutes;i++) { // run for a few hours and record in the SD card
    logfile.print(i);
    logfile.print(',');
    DateTime now = rtc.now();
    logfile.print(now.hour(), DEC);
    logfile.print(',');
    logfile.print(now.minute(), DEC);
    logfile.print(',');
   
    Vs = float(oversample16x()); // we read Vs as a digital number between 0-1023. It will turned to volts in the spreadsheet. 

    Serial.println(Vs); // write Vs
    logfile.println(Vs);
    // wait for 1min
    for (int j = 0; j < 60; j++)
    {
      delay(1000);
    }
    }
    
     //  digitalWrite(3,LOW);
        DDRD=DDRD&0b11110111; // turn off LED
      
        logfile.close(); // close file 
        while(1);
  }


void initADC(void) {
  ADMUX = ADMUX & 0xF0;                     /* set mux to ADC0 . The reading is on A0. */
  // that means the analog voltage is on A0
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC . So around 5V*/
  ADCSRA |= (1 << ADPS1) | (1 << ADPS2) | 1 << ADPS0; /* ADC clock prescaler /128 */
  ADCSRA |= (1 << ADEN);                                 /* enable ADC */
}

uint16_t oversample16x(void) { // record 16 data and then average them. It's running average.
  uint16_t oversampledValue = 0;
  uint8_t i;
  for (i = 0; i < 16; i++) {
    ADCSRA |= (1 << ADSC);                     /* start ADC conversion */
    loop_until_bit_is_clear(ADCSRA, ADSC);          /* wait until done */
    oversampledValue += ADC;                        /* add them up 16x */
  }
  return (oversampledValue >> 4);          /* divide back down by four */
}
