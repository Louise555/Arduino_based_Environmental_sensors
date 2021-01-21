# Arduino based air quality sensors
### Arduino based acquisition systems to monitor air quality - @ Veronique Lankar 2020 <br/>

The guideline to build those Arduino based air quality monitor is found here:                                                                                         https://leanpub.com/arduino_based_environmental_sensors                                                                                                                        Arduino Based Environmental acquisition systems - NO2, O3, CO, CO2/VOC, O2, UV, dust                                                                                            
### APPLICATION: monitoring the pollution during the lockdown in NYC – Spring 2020 <br/><br/>
### APPLICATION: monitoring the levels of O2, CO2, VOC in different masks (fabric, surg, N95) <br/><br/>

### mq131 (O3 sensor) from Winsen:  <br/>
-https://www.winsen-sensor.com/d/files/PDF/Semiconductor%20Gas%20Sensor/mq131(high-concentration)-ver1_4-manual.pdf <br/>
bought here:<br/>
https://www.ebay.com/itm/ORIGINAL-Brand-New-Winsen-MQ131-Ozone-Gas-Sensor/281782107363?hash=item419b85bce3:g:NqsAAOSwyQtV3Bxd<br/>
sensitivity curves:<br/>
https://github.com/ostaquet/Arduino-MQ131-driver<br/>

### MiCS6814 (NO2 sensor) from sensortech :<br/>
https://www.sgxsensortech.com/content/uploads/2015/02/1143_Datasheet-MiCS-6814-rev-8.pdf<br/>
bought here:<br/>
https://www.ebay.com/itm/MICS-6814-Chip-6814-Carbon-Monoxide-Nitrogen-Oxygen-Compact-Sensor-CO-NO2-NH3/173675327867?hash=item286fdb317b:g:84IAAOSwKM1cBsDE<br/>

### PM25 (dust sensor) from honeywell sold by Adafruit:<br/>
https://learn.adafruit.com/pm25-air-quality-sensor/overview <br/>

### VELM6075 (UV sensor) from Vishay sold by Adafruit:<br/>
https://learn.adafruit.com/adafruit-veml6075-uva-uvb-uv-index-sensor/overview (from Vishay)<br/>

### spg30 (CO2/VOC sensor) from Sensirion sold by Adafruit: <br/>
https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor <br/>
( Total Volatile Organic Compound (TVOC) reading and an equivalent carbon dioxide reading (eCO2) )<br/>

### MQ7 (CO sensor) from Winsen:<br/>
https://www.winsen-sensor.com/d/files/PDF/Semiconductor%20Gas%20Sensor/MQ-7B%20(Ver1.4)%20-%20Manual.pdf<br/>
bought here:<br/>
https://www.amazon.com/HiLetgo-Carbon-Monoxide-Detection-Sensor/dp/B00LSECY2Y/ref=sr_1_3?dchild=1&keywords=mq7&qid=1588261291&sr=8-3<br/>
explanation for hardware (the module baove needs to be modified): <br/>
https://www.instructables.com/id/Arduino-CO-Monitor-Using-MQ-7-Sensor/ <br/>


### ME2-O2-Ф20 (O2 sensor) from Winsen :<br/>
https://www.winsen-sensor.com/d/files/PDF/Electrochemical%20Gas%20Sensor/Electrochemical%20Oxygen/ME2-O2-D20%200-25%25%20Manual%20(ver1.2).pdf <br/>
module/code developed by: https://wiki.seeedstudio.com/Grove-Gas_Sensor-O2/<br/>
bought here: https://www.amazon.com/Air-Quality-Sensors-Grove-Sensor/dp/B01AUVL8HW <br/> <br/> <br/>

### datalogger test codes - For all the sensors we use the datalogging shield by adafruit:<br/>
https://learn.adafruit.com/adafruit-data-logger-shield<br/>

### for some sensors we use the temp/humidity sensor (by sensirion) arranged and sold by adafruit:<br/>
SHT31 (humidity/temperature sensor) from sensirion  sold by adafruit https://www.adafruit.com/product/2857 <br/>









