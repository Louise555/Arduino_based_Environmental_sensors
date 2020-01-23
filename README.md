# Arduino_based_Environmental_sensors
Arduino based acquisition systems to monitor air quality - Environmental Sciences Dept. Manhattan College, NY

mq131 -> ozone sensor from Winsen - https://www.ebay.com/itm/ORIGINAL-Brand-New-Winsen-MQ131-Ozone-Gas-Sensor/281782107363?hash=item419b85bce3:g:NqsAAOSwyQtV3Bxd

MiCS6814-> NO2 sensor from sensortech - https://www.sgxsensortech.com/content/uploads/2015/02/1143_Datasheet-MiCS-6814-rev-8.pdf

PM25 -> dust sensor https://learn.adafruit.com/pm25-air-quality-sensor/overview (from honeywell)

VELM6075 -> UV sensor  https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/overview (from Vishay)

spg30 -> Total Volatile Organic Compound (TVOC) reading and an equivalent carbon dioxide reading (eCO2) 
https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor (sensor from Sensirion)

Most of them take 20 minutes to collect the data. Except for the UV that takes like one minute. The ozone sensor takes 40 minutes because of warmup time. All of them keep track of the time and date. For the ozone sensor, you need to keep track of the humidity and temperature for calibration (using the website breezometer for example). The CO2/VOC measures the temperature itself but supposes an humidity of 50%. Modify the code to change the humidity. 





