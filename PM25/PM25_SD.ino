// 4/9/19   Sensor to measure the concentration of dust particle in air (classified by size)
//https://learn.adafruit.com/pm25-air-quality-sensor
//The data are recorded in a SD card and we keep track of the time. 
//We have extended the Arduino with a data-logger from adafruit. https://learn.adafruit.com/adafruit-data-logger-shield
// We record the concentration of particles in 0.1L larger than 0.3um, 0.5um, 1.0um, 2.5um, 5.0um and 10um (10 lines)
// We also record the date (year,month,day,hour,minute)
//
// We have a signaling LED  at pin 4 with a 330ohms resistor. 
// We use a daotalogger shield from adafruit to record data in a SD card (SPI bus) and to keep track of the time (I2C bus) with a Real TIme Clock. 
// we use scl and sda (I2C) for communication for the clock (A4 and A5 if Arduino). 
//
// we record 10 lines of data and wait for 5s between each measurement. 
// we record the date and time at the end if the file. 
//
// pin 2 (Rx arduino) and 3 (Tx arduino)  are used to communicate with the sensor through Serial communication (Rx,Tx) but only pin2 is connected to the sensor
// because nothing is sent to the sensor. the sensor is transmitting but not recieving.   for arduino a software serial is used. 
// The sensor has 3 connections. One for Tx -> Rx (pin 2), one for 5V, one for Gnd. 
//
// we use a SD card that is using pins 11, 12, 13 and 10  (chipselect) for SPI protocol (to transfer data from ARduino to the card)
// the clock uses pins A4 and A5 for  I2C bus
//
 

#include <SoftwareSerial.h> // header for the software Serial communication library between sensor and arduino
#include "SD.h" // header for the SD linrary
#include <Wire.h> // header for the I2c protocol
#include "RTClib.h" // for the clock

RTC_PCF8523 rtc; // instance of the clock. See adafruit. 

SoftwareSerial pmsSerial(2, 3); // Rx is pin 2. We only use pin 2

// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. tis is important. 

// the logging file that will be recorded in the SD card.
File logfile;


void setup() {
  
   pinMode(4, OUTPUT);  // this LED is on when it is recording.

  Serial.begin(115200); // this is the serial monitor. If arduino is pluged to the PC we can see on the monitor if the SD card is working 
   // and if the data are recorded (it will display 1 2.. 10)
    // sensor baud rate is 9600
  pmsSerial.begin(9600); // this is for SErial communication between arduino and the sensor. 

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
// from adafruit for the data sent by the sensor through Rx/Tx
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;
    int i=0;
void loop() {
      digitalWrite(4, HIGH); // turn on the light so we know it is recording. 
    if (readPMSdata(&pmsSerial)) {
    // reading data was successful!. I take only 10 measurements. I wait for 5 s between measurement,
    i=i+1;
    Serial.println(i);
    if (i>2) {
   // logfile.print(i-3);
   // logfile.print(",");
    logfile.print(data.particles_03um);
    logfile.print(",");
    logfile.print(data.particles_05um);
    logfile.print(",");
    logfile.print(data.particles_10um);
    logfile.print(",");
    logfile.print(data.particles_25um);
    logfile.print(",");
    logfile.print(data.particles_50um);
    logfile.print(",");
    logfile.println(data.particles_100um);
    }
    delay(5000);

  }
  if (i==12) {
      // uncomment to calibrate the time with computer .
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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
    logfile.close();
    digitalWrite(4, LOW);
      

  while(1){}} // when recording is done I turn off LED . I close the logfile and I do nothing. The loop is stopped. 
 
  
}
// This is from adafruit. TO work the sensor and detect errors. 
boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
   // Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
