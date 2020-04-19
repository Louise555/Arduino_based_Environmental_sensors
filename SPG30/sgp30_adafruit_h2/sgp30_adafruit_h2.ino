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

#include <Wire.h>  // libray I2C 
#include "Adafruit_SGP30.h"   // library sensor
#include "SD.h" // this is for the SD card
float temperature=15;
float humidity=50;

int number_data=20; // number of data to collect

#include "RTClib.h"

RTC_PCF8523 rtc; // This is for the new clock from adafruit. uncomment if you have the last version of the clock
//RTC_DS3231 rtc; // this is for the older version.


Adafruit_SGP30 sgp; // this creates a SPF30 object

int updateRet;

/* return absolute humidity [mg/m^3] with approximation formula
  @param temperature [°C]
  @param humidity [%RH]
*/

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

// for the data logging shield, pin 10 is for chip select for the SPI protocol. The other pins are 11,12,13
const int chipSelect = 10;  // we use pin 10. tis is important.

// the logging file that will be recorded in the SD card.
File logfile;

void setup() {

  Serial.begin(9600);

  //pinMode(LED_pin,OUTPUT);
  DDRB = DDRB | (1 << 0);  // this is the LED pin. at pin 8


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



  sgp.setIAQBaseline(0x96C1, 0x9047);  // Will vary for each sensor!

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
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  // trying clock
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime now = rtc.now();

  logfile.print(now.month(), DEC);
  logfile.print(',');
  logfile.println(now.day(), DEC);
  logfile.print(",");
  logfile.print(now.hour(), DEC);
  logfile.print(',');
  logfile.println(now.minute(), DEC);

// set humidity and temperature

  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));
}

int counter = 0;//

int counter2 = 0; // use another counter  to limit the numbers of deta recorded.
void loop() {
  //  digitalWrite(LED_pin,HIGH);
  PORTB = PORTB | (1 << 0);

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }

  logfile.print(sgp.TVOC); logfile.print(",");
  delay(2000);
  logfile.println(sgp.eCO2);
  Serial.print(sgp.TVOC); Serial.print(",");
  delay(2000);
  Serial.println(sgp.eCO2);
  counter2 = counter2 + 1;
  Serial.println(counter2);


  counter++; //

  delay(5000); 
    
  if (counter2 == number_data)  {
    Serial.println(" DONE");
    logfile.println(temperature);
    logfile.println(humidity);
    logfile.close();
    //digitalWrite(LED_pin,LOW);
    PORTB = PORTB & ~(1 << 0);
    while (1) {}
  }


}
