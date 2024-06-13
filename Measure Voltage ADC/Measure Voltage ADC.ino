#include "esp_adc_cal.h"

const int GPIOa =  13; 

const float alpha = 0.95; // Low Pass Filter alpha (0 - 1 )
const float aRef = 4.95; // analog reference
float filteredVal = 2047.0; // midway starting point

float R1 = 974.0;
float R2 = 2190.0;

int sensorVal = 0;
float voltage = 0.0;

float psiVal;
int angleVal; 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 


}

void loop() {


  sensorVal = analogRead(GPIOa);
  //voltage = readADC_Cal(sensorVal);
  //Serial.println(voltage/1000.0); // Print Voltage (in V)
  voltage = readADC_Cal(sensorVal) * (3.0/4095) * ((R1+R2)/R2);  // x=(y-b)/m


  Serial.print(sensorVal);  // prints a label
  Serial.print("\t");   
  Serial.print(voltage);  // prints a label
  Serial.print("\t");   
  Serial.print(voltage/1000.0);  // prints a label
  // Serial.print("\t");  
  // Serial.print(psiVal);  // prints a label
  // Serial.print("\t");  
  // Serial.print(angleVal);  // prints a label
  Serial.println();  

  delay(500);


}

uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
