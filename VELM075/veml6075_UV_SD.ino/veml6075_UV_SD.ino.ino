/*   
 * * 2/7/19 write in the SD card  year/month/day/hour/time then UVA,UVB,index
 * The sensor displays the levels of UVA and UVB (μW/cm^2 ) so the flux of UV light and the UV index. 
 * The calibration was done by Adafruit based on the datasheet of the sensor
 *
 *  see  https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/overview  for the wiring
 * Connect Vin to 5V  Gnd to Gnd and the I2c bus : SDA and CLK (arduino A4 and A5) 
 * I have added a 10nF capacitor between the 5V and Gnd. 
 * I have added a LED at digital 7 to turn on when the recording is done and off when it is over. pin 7 - LED - resistor 330 ohms - Gnd
 * Arduino takes 30 reading then the LED is turned off. The data are recorded in a SD card
 * 
 *    We use the datalogger from adafruit. Its an expansion " shield" that you add on arduino (like lego)
 *  https://learn.adafruit.com/adafruit-data-logger-shield/wiring-and-config
 *  The shield has a clock powered with a battery to keep track of the time.
*    so the pins A4 and A5 are for the I2C bus . It's a serial communication protocol between the clock and Arduino .
*    The pins 10,11,12,13 are for SPI bus . It's a Serial protocol so data can be sent from Arduino to the SD card.
*     we can not use the pins for other tasks.
 * 
 * commenta from adafruit: 
 * 
 * Designed specifically to work with the VEML6075 sensor from Adafruit
 * ----> https://www.adafruit.com/products/3964
 *
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.  
 *see  https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/overview  for the wiring
 * MIT license, all text here must be included in any redistribution.
 * See here for details: https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/arduino-test
 * https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/arduino-test
 * 
 * 
*     
 */
 
#include <Wire.h> // header for the I2C library 
#include "Adafruit_VEML6075.h" // header for the library of the UV sensor
#include "SD.h" // header for  the SD card
int counter=0; // to count 30 data

#include "RTClib.h" // header for the clock library
RTC_PCF8523 rtc; // for the clock . rtc is an instance

Adafruit_VEML6075 uv = Adafruit_VEML6075(); // for the uv sensor. 

// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. this is important. 

// the logging file that will be recorded in the SD card.
File logfile;

void setup() {
  pinMode(7,OUTPUT); // connecting a lED to pin 7 so it will turn on when recording data. you also need a resistance to protect it. 
  Serial.begin(9600); // baud rate
  // testing the I2C communication with the sensor
  Serial.println("VEML6075 Full Test");
  if (! uv.begin()) {
    Serial.println("Failed to communicate with VEML6075 sensor, check wiring?");
  }
  Serial.println("Found VEML6075 sensor");
  // Set the integration constant
  uv.setIntegrationTime(VEML6075_100MS);
  // Get the integration constant and print it!
  Serial.print("Integration time set to ");
  switch (uv.getIntegrationTime()) {
    case VEML6075_50MS: Serial.print("50"); break;
    case VEML6075_100MS: Serial.print("100"); break;
    case VEML6075_200MS: Serial.print("200"); break;
    case VEML6075_400MS: Serial.print("400"); break;
    case VEML6075_800MS: Serial.print("800"); break;
  }
  Serial.println("ms");
  // Set the high dynamic mode
  uv.setHighDynamic(false);
  // Get the mode
  if (uv.getHighDynamic()) {
    Serial.println("High dynamic reading mode");
  } else {
    Serial.println("Normal dynamic reading mode");
  }

  // Set the mode
  uv.setForcedMode(false);
  // Get the mode
  if (uv.getForcedMode()) {
    Serial.println("Forced reading mode");
  } else {
    Serial.println("Continuous reading mode");
  }
 // Set the calibration coefficients
  uv.setCoefficients(2.22, 1.33,  // UVA_A and UVA_B coefficients
                     2.95, 1.74,  // UVB_C and UVB_D coefficients
                     0.001461, 0.002591); // UVA and UVB responses



      // initialize the SD card. display on screen message error. 
   Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
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
  
// writing down the date and time in the SD card
    DateTime now = rtc.now();
    // uncomment to calibrate the time with computer .
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    logfile.print(now.month(), DEC);
    logfile.print(',');
    logfile.println(now.day(), DEC);
    logfile.print(",");
    logfile.print(now.hour(), DEC);
    logfile.print(',');
    logfile.println(now.minute(), DEC);
 
}


void loop() {
  delay(1000);
  counter=counter+1;
  Serial.println(counter);
  digitalWrite(7,HIGH); // turn on the LED
logfile.print(uv.readUVA());logfile.print(',');
logfile.print(uv.readUVB());logfile.print(',');
logfile.println(uv.readUVI());
if (counter==30) {Serial.print("DONE");logfile.close();digitalWrite(7,LOW);while(1){} } // turning off the LED closing the file

  delay(1000);
}
