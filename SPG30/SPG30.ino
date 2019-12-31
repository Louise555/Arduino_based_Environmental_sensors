// 5/2/19 V. Lankar 
// The breakout and drier is from: https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
/*
 *  The sensor measures a Total Volatile Organic Compound (TVOC) reading and an equivalent carbon dioxide reading (eCO2) 
 *  The sensor measures  eCO2 (equivalent calculated carbon-dioxide) concentration within a range of 400 to 60,000 parts per million (ppm), 
 *  and TVOC (Total Volatile Organic Compound) concentration within a range of 0 to 60,000 parts per billion (ppb). 
 *  It offers a typical resolution of 15% within measured values. 
 */
// the sensor is humidity and temperature dependent. So we have extended the system with a temperature sensor TMP36 (pin A0)  that measures temperature.
// https://learn.adafruit.com/tmp36-temperature-sensor
// For the humidity, the code suppose a humidity of 50% to be changed accordingly (the system was too slow when we tried a humidity-temperature sensor).
//
// we have extended Arduino with a data-logger from adafruit so data are recorded in a SD card and we also record the date and time
// (thanks to the clock included in the datalogger). We record 90 lines of data:
// the header has the month,day,time,munite. 
// Then the TVOC and eCO2. Important: 
//It takes a few seconds for the sensor to be ready to function and during that time a series of 0,400 are written.
// Those lines have to be disregraded when processing the data. 
// the footer contains the temperature and the humidity. 
// we have also added a signaling LED at digital pin 8. 
//
// Note: for the data-logger. Pins 10,11,12,13 are used for the SPI serial bus between the SD card and Arduino
// pins A4 and A5 are used for the I2C bus for the clock.
// pin A0 is ued for the TMP36. 
//
// if running the sensor for the first time, you need to find the baseline values: https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
// to be used in : sgp.setIAQBaseline(0x8F3C, 0x8FEF);


#include <Wire.h>  // header for I2C 
#include "Adafruit_SGP30.h"   // header for the driver 
#include "SD.h" // header for the SD card
float temperature; // will be measured by the TMP36
float humidity=50.; // can be changed. 
int sensorPin = 0; //the analog pin the TMP36's Vout pin is connected to A0
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures


#include "RTClib.h" // header for the clock
RTC_PCF8523 rtc; // This is for the new clock (RTC)  from adafruit. 
//RTC_DS3231 rtc; // this is for the older version. 

// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. tis is important. 

// the logging file that will be recorded in the SD card.
File logfile;



Adafruit_SGP30 sgp; // this creates a SPF30 object

int updateRet;

/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [°C]
* @param humidity [%RH]
*/

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}


void setup() {

    Serial.begin(9600);
    
//pinMode(LED_pin,OUTPUT);
DDRB=DDRB|(1<<0); // digital pin 8


// read temperature with the TMP36. Its an analog sensor. it uses A0 from arduino. 
 int reading = analogRead(sensorPin); 
  float voltage = reading * 5.0;
 voltage /= 1024.0; 
 temperature=(voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
 Serial.print(temperature); Serial.println(" degrees C");

    // testing SPG30
    
  Serial.println("SGP30 test");

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
   while (1); 
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  // otherwise you have to run the sensor outside to get the baseline. Uncomments the 2 lines at the end of the code.
  // 1/24/19 I let the program run for 10 minutes outside to get number for baselines. and I get ****Baseline values: eCO2: 0x8F3D & TVOC: 0x8FF0
  // I followed instructions from adafruit. https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code


  sgp.setIAQBaseline(0x8F3C, 0x8FEF);  // Will vary for each sensor!

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
  
// clock 
  // uncomment to calibrate the time with computer .
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    DateTime now = rtc.now();
   
    logfile.print(now.month(), DEC);
    logfile.print(',');
    logfile.println(now.day(), DEC);
    logfile.print(",");
    logfile.print(now.hour(), DEC);
    logfile.print(',');
    logfile.println(now.minute(), DEC);


// compensafe for the humidity and temperature. 

  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));
}

int counter = 0;

int counter2 =0; // we stop recording when we have recorded 90 data. Skip the 0,400 records. 
void loop() {
 //  digitalWrite(LED_pin,HIGH);
 PORTB=PORTB|(1<<0);

  if (! sgp.IAQmeasure()) {  
   Serial.println("Measurement failed");
   return;
 }
  // writing the numbers only if the temprature was found
//  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
//  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
  logfile.print(sgp.TVOC); logfile.print(",");
  logfile.println(sgp.eCO2);
 Serial.print(sgp.TVOC); Serial.print(",");
  Serial.println(sgp.eCO2);
  counter2= counter2+1;
  Serial.println(counter2);
  
 
  counter++;
 
  delay(1000);
    if (counter2==90)  {
      Serial.println(" DONE");
    logfile.println(temperature);
    logfile.println(humidity);
    logfile.close();
    //digitalWrite(LED_pin,LOW);
    PORTB=PORTB&~(1<<0);
    while(1) {}}
 // baseline to get when first using the sensor: 
 //   Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
 //   Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
    
  }
  
