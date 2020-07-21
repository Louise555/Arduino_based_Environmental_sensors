// V. Lankar -code to continuously dispay VOC and CO2 and to continuously measure temperature and humidity with the SHT31 from adafruit. 
// we still use the datalogger but we are not recording in the SD card. 
// this code is a variation of the SD one. for experiments that do not require to record in SD card but require a continuous monitoring.
// we kept the datalogger for convenience. 
// the data is displayed every 5s x12 = 1minutes  . we average 12 data
// the SPG30 needs the temperature and humidity to function 
// 
// the SHT31 from adaffruit. https://learn.adafruit.com/adafruit-sht31-d-temperature-and-humidity-sensor-breakout/wiring-and-test
// The sensor uses also the protocol I2C like the SPG30. 
//
// If the module does not run from a long time, you need to run the test code to get new baseline data
// typical values:  ****Baseline values: eCO2: 0x96C1 & TVOC: 0x9047
// 
// The module is from adafruit: 
//  https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
// spg30_adafruit.ino   the 4/19 I found ****Baseline values: eCO2: 0x96C1 & TVOC: 0x9047
// typical readings: TVOC 969 ppb eCO2 1340 ppm example of reading
//

#include <Wire.h>  // libray I2C communication. This is used for the clock and also for our temp/humidity sensor. 
#include "Adafruit_SGP30.h"   // library sensor
#include "SD.h" // this is for the SD card
// below the numbers will be changed.  
float temperature=15; // if the sensor does not work the temperatuer is 15C
float humidity =50; // if the sensor SHT31 does not work the humidity is 50 %


#include "RTClib.h" // this is the header for the clock. 

RTC_PCF8523 rtc; // This is for the new clock from adafruit. uncomment if you have the last version of the clock
//RTC_DS3231 rtc; // this is for the older version.


Adafruit_SGP30 sgp; // this creates a SPG30 object. This is the sensor for VOC and CO2

// below this is for the temp/humidity sensor from adafruit. It uses I2C
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();



/* return absolute humidity [mg/m^3] with approximation formula
  @param temperature [°C]
  @param humidity [%RH]
*/
// this is a structure for the spg30 sensor. 

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. tis is important.


void setup() {
 
  Serial.begin(9600);
  Serial.println("  HELLO  ");

  //pinMode(LED_pin,OUTPUT);
  DDRB = DDRB | (1 << 0);  // this is the LED pin. at pin 8
  
// below testing the sensor SPG 30
  Serial.println("SGP30 test");

  if (! sgp.begin()) {
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);



// to get these numbers run the test code. It is necessary for calibration. 
  sgp.setIAQBaseline(0x96C1, 0x9047);  // Will vary for each sensor!

  // below is for the SD card but we don't use it anymore. I removed the opening of files. 
  // initialize the SD card. display on screen message error.
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT); // this is for SPI  communication for the SD card

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

// 

  // this is for the clock. If the clock is not right (month,day,time) uncomment the line below. run the code.
  // then comment again and upload again. 
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
// getting the date of today. We don't write it in the SD card anymore. 
  DateTime now = rtc.now();

  Serial.print(now.month(), DEC);
  Serial.print(',');
  Serial.print(now.day(), DEC);
  Serial.print(",");
  Serial.print(now.hour(), DEC);
  Serial.print(',');
  Serial.println(now.minute(), DEC);

}


int counter2 = 1; // use counter  to  keep track of the number of data to average. we average 12 data. and then display them.
// we wait 5s between each data recorded. so 60s btween each display. 
// we will take the average of VOC and CO2 :
float total_VOC=0;
float total_CO2=0;

void loop() {
  //  digitalWrite(LED_pin,HIGH);
  PORTB = PORTB | (1 << 0);

 // first we measure temperature and humidity using the sensor sht31  . We need it for the VOC/CO2 
 // ---------------------------------
sht31.begin(0x44);
  
   temperature= sht31.readTemperature();
  humidity = sht31.readHumidity();

  if (! isnan(temperature)) {  // check if 'is not a number'
   // Serial.print("Temp *C = "); Serial.println(t);

  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(humidity)) {  // check if 'is not a number'
  //  Serial.print("Hum. % = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read humidity");
  }
  Wire.endTransmission(0x44);
  // ----------------------

  //
  // we use  humidity and temperature for our SPG30 sensor. 

  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  // testing if it reads every thing properly. comment if it does
  //Serial.print(temperature);
 // Serial.print(' ');
 // Serial.print(humidity);
  //  Serial.print(' ');
  //Serial.print(sgp.TVOC);
  //Serial.print(' ');
  //Serial.println(sgp.eCO2);
// we don't want to record data with eCO2=400. They are not good data. 
  if (sgp.eCO2!=400) {
  total_VOC=total_VOC+sgp.TVOC;
  total_CO2=total_CO2+sgp.eCO2;
    counter2 = counter2 + 1;
  }

// if 60 minutes has pased then we display the temperature, humidity, VOC, CO2
  if (counter2==12) {
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(total_VOC/12);
  Serial.print(",");
  delay(2000);
  Serial.println(total_CO2/12);
 total_VOC=0;
 total_CO2=0;
  counter2=0;
  } 



  delay(5000);

}
