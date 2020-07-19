/*
 *  * ME2-O2-Ф20 electrochemical sensor
 * The module (that includes a sensor)  was developped by: 
 * https://wiki.seeedstudio.com/Grove-Gas_Sensor-O2/
 * 
 * The connections with the module are very easy. One for Vin (5V), one for Gnd, one for A0 of arduino 
 * The code is provided by Grove (above) and turn the voltage measured  to concentration of O2 in %. 
 * We expect a concentration of 20.9% outside. You can tune the calibration by changing the int calibration = 2;
 * We can try 1.9 fro example if the concentration is too small. 
 * 
 * First we run the code for 5 minutes to heat up and stabilize. During that time the concentration will decrease toa stable value.
 * Then we display the concentration every 30s continuously. 
 * In both phases, the data are averaged over 32 measurements. 
 * 
 * datasheet found here:
 * https://www.winsen-sensor.com/sensors/o2-sensor/me2-o2.html
 * https://www.winsen-sensor.com/d/files/PDF/Electrochemical%20Gas%20Sensor/Electrochemical%20Oxygen/ME2-O2-D20%200-25%25%20Manual%20(ver1.2).pdf
 * 
 * we bought the module from: 
 * https://www.amazon.com/Air-Quality-Sensors-Grove-Sensor/dp/B01AUVL8HW
 * 
 * Please power the Gas Sensor(O2) more than 48 hrs before you get data from it.
 * THe sensor needs  10 minutes to heat
 */

// Grove - Gas Sensor(O2) test code
//https://wiki.seeedstudio.com/Grove-Gas_Sensor-O2/
// Note:
// 1. It need about about 5-10 minutes to preheat the sensor and48 hours before.
// it needs to be calibrated before. see calibration code. 
// 2. modify VRefer if needed

 
const float VRefer = 5;       // voltage of adc reference
const int pinAdc   = A0;
// to tune the sensor change 2 to 1.8 or 1.9 (or around this number) so you get 20.9% of O2 in clean air. 
int calibration = 2;
void setup() 
{
   // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println(" Hello ");
    Serial.println("");
    Serial.print(" the calibration number is ");
    Serial.println(calibration);
    Serial.println(" warming up for 5 minutes ");
    for (int j;j<600;j++){
      Serial.print("  Concentration of O2 is ");
      Serial.println(readConcentration());
      delay(500);
    }
    Serial.println(" done warming up");
}

 void loop() 

{

    // put your main code here, to run repeatedly:
   float Vout =0;
//   Serial.print("Vout =");
    Vout = readO2Vout();
//   Serial.print(Vout);
 //  Serial.print(" V, Concentration of O2 is ");
  Serial.println(readConcentration());
   // wait for 30 s
   for (int j=0;j<60;j++){
    delay(500);
   }
}

 

float readO2Vout()

{
   long sum = 0;
   for(int i=0; i<32; i++)

    {

        sum += analogRead(pinAdc);

    }

    sum >>= 5; // divide by 32 (right shift)

    float MeasuredVout = sum * (VRefer / 1023.0);
   return MeasuredVout;
}

float readConcentration()

{

    // Vout samples are with reference to 3.3V

    float MeasuredVout = readO2Vout();

    //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);

    //when its output voltage is 2.0V,

 //   float Concentration = MeasuredVout * 0.21 / 2.0;
 // change the 2 to adjust the calibration
float Concentration = MeasuredVout * 0.21 / calibration;
    float Concentration_Percentage=Concentration*100;

    return Concentration_Percentage;

}
