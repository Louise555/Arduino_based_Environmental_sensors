// record in the SD card the date/time then a series of data  VOC (ppb), CO2(ppm) then the temperature and humidity. 
//  TVOC 969 ppb eCO2 1340 ppm is an example of reading
//
// the sensor for C02  SGP30  and organic matter comes from:
// https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
// the C02 sensor uses I2C communication. The lines SDA and SCL are used
// to work well the sneosr needs the basline numbers:
 // for example  sgp.setIAQBaseline(0x96B8, 0x90CF); here // Will vary for each sensor!
 // to get those numbers, needed for calibration, run the test code from ada fruit for a few hours outside,
//
// We use the SHT31 from adafruit as a temperature/humidity sensor. It works well.
//  https://learn.adafruit.com/adafruit-sht31-d-temperature-and-humidity-sensor-breakout/wiring-and-test
//  However, both sensors use the I2C bus. So we read first teh temperature and the humidity and then we close the communication
// If the sensor SHT31 does not work then we take a temp by default = 17C  and humidity = 50%

//  if not used for a long time you need to run it outside and get a new base line. See the test code by adafruit. 
//
// number_data is the nubmer of data to collect. 
// the module  for VOC,CO2 is developed for adafruit. 
//  https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code
// The data logger is also from adafruit.
// https://learn.adafruit.com/adafruit-data-logger-shield
// The shield uses the SPI protocol for the  SD card  at 10 (slave select) 11, 12 , 13  (clck). The clock uses   I2C  -> pins A4 and A5 
//
// note that spg30 also uses I2C and the SHT31 as well
//
//
// There is now an LED that turns on when the recording starts and turn off when we stop recording 


#include <Wire.h>  // libray I2C 
#include "Adafruit_SGP30.h"   // library sensor
#include "SD.h" // this is for the SD card
float temperature=15; // if the sensor does not work the temperatuer is 15C
float humidity=50; // if the sensor SHT31 does not work the humidity is 50 %

int number_data=20; // number of data to collect

#include "RTClib.h"

RTC_PCF8523 rtc; // This is for the new clock from adafruit. uncomment if you have the last version of the clock
//RTC_DS3231 rtc; // this is for the older version.


Adafruit_SGP30 sgp; // this creates a SPF30 object
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();

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

sht31.begin(0x44);
  
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(t);
    temperature =t;
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
    humidity=h;
  } else { 
    Serial.println("Failed to read humidity");
  }
  Wire.endTransmission(0x44);
  

  Serial.println("SGP30 test");

  if (! sgp.begin()) {
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);


// run the initial code by adafruit to get those values. It will depends on the sensor. THose numbers are needed for the calibration
  sgp.setIAQBaseline(0x96B8, 0x90CF);  // Will vary for each sensor!



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

  // uncomment below to adjust the clock. 
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DateTime now = rtc.now();

  logfile.print(now.month(), DEC);
  logfile.print(',');
  logfile.print(now.day(), DEC);
  logfile.print(",");
  logfile.print(now.hour(), DEC);
  logfile.print(',');
  logfile.println(now.minute(), DEC);

  Serial.print(now.month(), DEC);
  Serial.print(',');
  Serial.print(now.day(), DEC);
  Serial.print(",");
  Serial.print(now.hour(), DEC);
  Serial.print(',');
  Serial.println(now.minute(), DEC);
// set humidity and temperature

  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));
}


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
