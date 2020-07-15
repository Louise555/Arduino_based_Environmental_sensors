// 5/6/2020 Veronique Lankar @2020 testing the sensor for CO. bought at amazon
//HiLetgo 2pcs MQ-7 Carbon Monoxide Detection Sensor Module CO Gas Sensor Module
// based on the intructions below that are based on the datasheet below.
//https://cdn.sparkfun.com/datasheets/Sensors/Biometric/MQ-7%20Ver1.3%20-%20Manual.pdf
//
//The circuitry is based on :
// https://www.instructables.com/id/Arduino-CO-Monitor-Using-MQ-7-Sensor/
// this website gives also the instructions to use the sensor properly. 
/*I followed the circuitry of the website. But I used a MOSFET 2N7000 instead of a BJT. Applying a 5V or 0V turn on or off the transistor
 * there is no need for additional resistor at the base for the transistor.
 * I skipped the part that includes LED and buzzer to warn high level of CO
 * For the MQ7 to work, it needs a 5V supply source for 60 seconds in order to clean the gases. 
 * then a 1.5V supply source for 90 seconds in order to collect the data (level of CO). 
 * This is so that the temperature will change. high temperature to clean the sensor. Low temperature to read Rs.
 * The resistance of the sensor Rs depends on the CO level. Then sensitivity curves from the datasheet can be used to calibrate the sensor.
 * The calibration will be done in a spreadsheet. This code will just read the voltage Vs across the load RL=10K.
 * So the components to get: 10K resistor for the load. 2 capacitors of 0.1 uF for the noise. the mosfet. 
 * 
 * To be able to use the breakout from flying-fish , 2 components had to be removed. A load resistor and a capacitor.(see website)
 * This is because the breakout was using the same power source for the heater and for the circuit 5V- (output pin) RL-Gnd
 * and this is wrong. Because the power source for the heater has to switch from 5V to 1.5V. 
 *So we have 2 circuits:
 *5V -> cap ->pin from breakout (Rs) and pin A0 of arduino  ->  RL -> Gnd
 * 5V -> breakout (Vin -Gnd) -> transistor -> Gnd. And there is a cap across the breakout. 
 * 
 * We connect the base of the transistor to a PWM of arduino. The frequency is 62.5KHz. This is done by an arduinot
 * library. TimerOne. We use pin 9 which is connected to timer1 (we can use also pin 10.
 * The frequency is set my the Timer1.initialize method. The input is the period in us (1/62500 in us).
 * So the current can vary at the same frequency. So the breakout (like a load) has a voltage drop varrying at the same
 * frequency. When the transistor is 0n the voltage drops is 5V and when the transistor is off the voltage drop is 0.
 * The duty cycle is controlled by pin 9. with Timer1.pwm(9, 1023) we get a voltage across the breakout of 5V
 * and with   Timer1.pwm(9, 330) we get a voltage of about 1.5V. I checked with the voltmeter.
 * It works. You can test the sensor by lightning a match under a cup. The voltage across the 10K load will change when you
 * place the sensor under the cup. 
 */

#include <TimerOne.h>
int sensorValue = 0; 
void setup() 
{
    Serial.begin(9600);
  pinMode(9, OUTPUT);   // we use pin9
  // we set the period of the PWM mode for pin 9 and 10. 16 us period means a 62,500 Hz frequency as suggested in the paper
    Timer1.initialize(16); // timer 1 -  units in microseconds for the period . f=62,500Hz T=16us 
  pinMode(A0, INPUT); // for the reading
}

void loop()
{
  
      Serial.println(" clearing the sensor of gases");
  // Apply 5V for 60 seconds, don't use these readings for CO measurement. 
    Timer1.pwm(9, 1023); // that should be 5V
  for (int i=0;i++;i<60){
  delay(1000);
  }
  // Apply 1.4V for 90 seconds, use these readings for CO measurement.
  int j=0;
  Timer1.pwm(9, 330); // that should be 1.4V
  while(j<90) {
  Serial.println("reading");
   Serial.println(analogRead(A0));
  delay(1000);
  j++;
  }
    
 
  
  
}
