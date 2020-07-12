
//simple code to test the row sensor mq131. It only output Rs the resistance of the sensor. The circuitry includes a load resistance RL. (5V -( Rs ) - RL - Gnd)
// (Rs) is the sensor. 
// We read the voltage across RL and turns it to Rs/ You can copy Rs and paste it in the excel sheet I have prepared. Copy the output in the
//column Rs. You also need to enter the right temperature in the temperature column. The concentration of ozone is read at the end as a function of
//humidity. So you need the temperature and humidity when you are using the sensor. You can add a sensor for humidity and temperature like the SHT31 from adafruit.
// You can skip the columns that have the times (excel sheet) . These are used with the code that uses a data logger. 
// We have found R0 to be 235000 but it will vary with sensor. R0 is the resistance for clean air. (see below) 
/*
 * Veronique Lankar @ 2019
 * the sensor was bought from ebay. It is manufactured by Winsen. 
 * https://www.ebay.com/itm/ORIGINAL-Brand-New-Winsen-MQ131-Ozone-Gas-Sensor-/281782107363
 * the datasheet with sensitivity curves  can be found here: 
 * https://github.com/ostaquet/Arduino-MQ131-driver/tree/master/datasheet
 * digitalized sensitivity curves can be found here: 
 * https://github.com/ostaquet/Arduino-MQ131-driver 
 * I didn't follow their circuitry but the sensitivity curves are correct. 
 * The black sensor  are LOW concentration and the metal ones are for HIGH concentration. We didn't find significant difference between them. 
 * Refer to the datasheet to understand the connections. The sensor is connected to 5V (and Gnd) from Arduino. Do not connect the Vin of the sensor to a pin. 
 * The sensor needs  more current that a pin can provide. 
 * 
 * The sensor needs to run for 48 hours before it can work properly. 
 * 
 * Also, the sensor requires 10-20 min to preheat. Before it can provide consistent data. 
 * 
 * The sensor is also connected to analog pin A0 so the digital voltage is read. The sensor has a resistance Rs that depends on the
 * ozone's concentration. So we have a voltage divider. 5V - Rs - RL - Gnd. We are reading the voltage RL  across a load resistor RL 
 * then we can find (voltage divider equation) Rs as a function of VL read at A0. (simple equation). 
 * 
 * The voltage VL gives us Rs. The resistance of the sensor. To get the concentration, Rs is divided by R0. 
 * R0 is the resistance of the sensor in clean air with a temperature of 20C and humidity of 60%.
 * Rs/R0 will give the concentration of ozone by using the sensitivity curves provided by the datasheet (or the above website). 
 * The concentration depends on the temperature and humidity. 
 * 
 * I have prepared an excel sheet that will compute the final concentrations as a function of temperature and humidity. 
 * 
*/
// - Load resistance RL of about  10KOhms (10000 Ohms)



float Rs;
int pinSensor=A0;
int valueRL=10000;


 




void setup() {

  Serial.begin(9600);
  pinMode(pinSensor, INPUT);

    
}

void loop() {



        
          
          Rs= (analogRead(pinSensor)/ 1024.0)*5.0; // measure the voltage across the load 10K and find the resistance of the sensor. 
          Rs = (5.0 / Rs - 1.0) * valueRL;
          Serial.println(Rs);

 
          delay(5000);
         
        



}
