// this version is to display VOC and CO2 continuously and average over 16 data
// we skip 0 and 400. We use the sensor SHT31 from adafruit. 
// 7/3/20  I removed the temperature sensor that was damaged. we supposed temp is 36 and hum 85 because the sensor will be placed inside the mask. 
//6/15/20 I modified the code again so it displays temp,humidity,VOC,CO2 every 60s about. And the VOC and CO2 are now averages. I average 12 data.
// we wait 5s before reading any daya (temp, humidity, VOC , CO2). 12 x 5=60s. so we average 12 data evrey 5s and then display. 
// Also I don't record anymore in the SD card. So I dont open files anymore in the SD card. 
// Also since the SPG30 needs the temp and humidity for its measurements I use the temp and humidity conituously for the SPG30. 
// Before we measured humidity and temperature only once before reading the VOC and CO2. (since we didn't expect a change over the time we were measuring. 
// 
// 6/14/20  I modified the code so it continuously displays counter,temperature,humidity,VOC, CO2. we are not using the SD card any more
// so the code has be modified so we read now the temperature and the humidity not only before the loop() but also inside the loop
// so it is continously reading temperature and humidity. 
// 4/27/20 I have added succesively a temperature/humidity sensor. 
// the SHT31 from adaffruit. https://learn.adafruit.com/adafruit-sht31-d-temperature-and-humidity-sensor-breakout/wiring-and-test
// It works well. However, both sensors use the I2C bus. So we read first teh temperature and the humidity and then we close the communication
// If the sensor SHT31 does not work then we take a temp by default and humidity = 50%

// 4/19/10 warning / if not used for a long time you need to run it outside and get a new base line.
// tempeature sensor does not work. I supposed temperature of 15C to be modified
// 5/2/19 humidity is 50%
// number_data is the nubmer of data to collect. 
// I removed the temperature sensor because it competes with the sensor for current. 
// If the sensor is not used for a few days you need to check the baselines number.
// for that run the test code by adafruit https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
// spg30_adafruit.ino   the 4/19 I found ****Baseline values: eCO2: 0x96C1 & TVOC: 0x9047
// finally got the kit C02 (sgp30 from adafruit) and the temperature to work at the same time and for to record the data in the SD card.
// had to make some change in the code.
// first make sure there is a connection between the ICPS headers (for SPI) between arduino and the shield. Otherwise, they say to flip the shield and make a path for SPI.
// arduino uno has SPI (for SD card)  at 10 (slave select) 11, 12 , 13  (clck).   I2C (for clock and sensor spg30) uses A4 and A5 for arduino R3
// temperature sensor TMT36 uses A0
// Then I read  he temperature once in setup() . So I don't start the SD card, the clock, the C02 until temperature is read.
// temperature is read for the C02 sensor. I set humidity to 50%.
//
// TVOC 969 ppb eCO2 1340 ppm example of reading

// There is now an LED that turns on when the loop starts and turn off when we stop recording (90 data).
// I un commented lines to include temperature and humidity to improve the Co2 sensor as recommended by adafruit.

// the sensor for C02  SGP30  and organic matter comes from:
// https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
// the C02 sensor uses I2C communication. The lines SDA and SCL are used

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
  Serial.println("  HELLO YELDA ! ");

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

  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  // 1/24/19 I let the program run for 10 minutes outside to get number for baselines. 
// **Baseline values: eCO2: 0x96C1 & TVOC: 0x9047
// I followed instructions from adafruit. ****Baseline values: eCO2: 0x96C1 & TVOC: 0x9047

// if the sensor was not used for a long time. You need to recalibrate and run the code provided by adafruit so get the baselines. 

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
 // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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

// we record 12 data and averaget them. We skip the 400 data. 

int counter2 = 1;
float total_VOC=0;
float total_CO2=0;

void loop() {
  //  digitalWrite(LED_pin,HIGH);
  PORTB = PORTB | (1 << 0);
  float temperature=15; // if the sensor does not work the temperatuer is 15C
  float humidity =50; // if the sensor SHT31 does not work the humidity is 50 %


  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
    Serial.print(sgp.TVOC); // 
  Serial.print(",");
  delay(1000);
  Serial.println(sgp.eCO2); //
  delay(1000);
// we don't want to record data with eCO2=400. They are not good data. 
  if (sgp.eCO2!=400) {
  total_VOC=total_VOC+sgp.TVOC;
  total_CO2=total_CO2+sgp.eCO2;
    counter2 = counter2 + 1;
  }

// if 60 minutes has pased then we display the temperature, humidity, VOC, CO2
  if (counter2==16) {
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(int(total_VOC>>4)); //we divide by 16
  Serial.print(",");
  delay(2000);
  Serial.println(int(total_CO2>>4));// we divide by 16
 total_VOC=0;
 total_CO2=0;
  counter2=0;
  } 

  //increasing the counter


  delay(4000);

}
