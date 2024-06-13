#include <SPI.h>
#include <TFT_eSPI.h> 
#include "esp_adc_cal.h"
#include "Fonts.h"
#include "Dials.h"
#include "SplashScreens.h"

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite dialFace = TFT_eSprite(&tft);

/*
PIN OUT FOR ESP32
dc-22
res-33
sda-15
scl-14
vcc-3.3v
*/
//cs pins
#define CS_psi 5
#define CS_temp 0
#define CS_erg 4

//defines the number of samples to be taken for a smooth average
#define NUMSAMPLES 5               
const float steinconstA = 0.00149923249973363;        //steinhart equation constant A, determined from wikipedia equations
const float steinconstB = 0.00020941020003873;       //steinhart equation constant B, determined from wikipedia equations
const float steinconstC = 0.00000057045424308;    //steinhart equation constant C, determined from wikipedia equations
int samples[NUMSAMPLES]; 

//defines the resistor value that is in series in the sensorPsiVoltage divider
#define coolantsensorDivider 2348   
float R1 = 974.0;
float R2 = 2190.0;

#define FILTER_LEN  50
uint32_t AN_Pot1_Buffer[FILTER_LEN] = {0};
int AN_Pot1_i = 0;

//tick array radius
const int OuterRadius = 100;
const int InnerRadius = 20;

const double rad = PI / 180;

//screen offsets
const int xoffset=120;
const int yoffset=120;

//pointer marker offsets
int markerOffset1;
int markerOffset2;

int tempMarkerOffset1;
int tempMarkerOffset2;

//##### PSI VARS
//0 to 100 dial variables
const float Dial100_AngleIncrement = 2.4;
float psi = 150; //330 degrees
int PIS_AngleVal; 

float x_psi[360];
float y_psi[360];
float x2_psi[360];
float y2_psi[360];

//psi variables
int sensorPSI = 0;
int sensorPsiAveraged = 0;
float sensorPsiVoltage = 0.0;
int psiVal = 0;


const int GPIO_PSI =  12; //psi inout


//##### TEMP VARS
//0 to 120 dial variables
const float Dial120_AngleIncrement = 2;

float temp = 150; //330 degrees
int TEMP_AngleVal; 

float x_temp[360];
float y_temp[360];
float x2_temp[360];
float y2_temp[360];

//temp variables
int sensorTemp = 0;
int sensorTempAveraged = 0;
float sensorTempVoltage = 0.0;
int tempVal = 0;

const int GPIO_TEMP =  15; //psi inout


//##### ERG VARS
const int GPIO_ERG =  13; //psi inout

float erg = 150; //330 degrees
float x_erg[360];
float y_erg[360];
float x2_erg[360];
float y2_erg[360];





void setup() {

  
  Serial.begin(9600); 


  pinMode(CS_psi, OUTPUT);
  digitalWrite(CS_psi, HIGH);
  
  pinMode(CS_temp, OUTPUT);
  digitalWrite(CS_temp, HIGH);

  pinMode(CS_erg, OUTPUT);
  digitalWrite(CS_erg, HIGH);

  // We need to 'init' both displays
  // at the same time. so set both cs pins low
  digitalWrite(CS_psi, LOW);
  digitalWrite(CS_temp, LOW);
  digitalWrite(CS_erg, LOW);

  //set up tft object
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setPivot(120,120);

  digitalWrite(CS_psi, HIGH);
  digitalWrite(CS_temp, HIGH);
  digitalWrite(CS_erg, HIGH);

  RunSplashScreen();

  ConfigureDials();

  //build the tick object from the required angle point
  SetupPSITickArray(psi);
  SetupTEMPTickArray(temp);
  SetupERGTickArray(erg);
  
}

void loop() {
  
  ProcessPSI();
  ProcessTEMP();
  //ProcessERG();

  //ProcessTEST();

  delay(1000);
}


void RunSplashScreen() {

  dialFace.setColorDepth(8);
  dialFace.setSwapBytes(true);
  dialFace.createSprite(240, 240);

  digitalWrite(CS_psi, LOW);
  dialFace.pushImage(0,0,240,240,SplashZ);
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_psi, HIGH);

  digitalWrite(CS_temp, LOW);
  dialFace.pushImage(0,0,240,240,SplashE);
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_temp, HIGH);

  digitalWrite(CS_erg, LOW);
  dialFace.pushImage(0,0,240,240,SplashD);
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_erg, HIGH);

  delay(4000);
}

void ConfigureDials() {

  dialFace.deleteSprite();

  dialFace.setColorDepth(8);
  dialFace.setSwapBytes(true);
  dialFace.createSprite(240, 240);
  dialFace.setTextDatum(4);
  dialFace.setTextColor(TFT_WHITE,TFT_BLACK);
  dialFace.setFreeFont(&FreeSansOblique9pt7b);

  digitalWrite(CS_psi, LOW);
  dialFace.pushImage(0,0,240,240,Dial_psi);
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_psi, HIGH);

  digitalWrite(CS_temp, LOW);
  dialFace.pushImage(0,0,240,240,Dial_temp_120);
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_temp, HIGH);

  digitalWrite(CS_erg, LOW);
  dialFace.pushImage(0,0,240,240,Dial_erg);
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_erg, HIGH);


}


void ProcessPSI() {
  
  digitalWrite(CS_psi, LOW);
  //push fresh copy of image to remove last needle and prevent strobing
  dialFace.pushImage(0,0,240,240,Dial_psi);
  digitalWrite(CS_psi, HIGH);

  sensorPSI = (int)analogRead(GPIO_PSI); // Read pressure sensor val (A0)
  Serial.print(sensorPSI);
  Serial.println();
  sensorPsiAveraged = readADC_Avg(sensorPSI);
  sensorPsiVoltage = readADC_Cal(sensorPsiAveraged) * (4.5/4095) * ((R1+R2)/R2);  // x_psi=(y_psi-b)/m
  psiVal = (sensorPsiVoltage - 0.5032) / 0.0401; // x_psi=(y_psi-b)/m

  //catch all for negative psi values
  if(psiVal < 0) {psiVal=0;}
  //catch all for negative psi values
  if(psiVal > 100) {psiVal=100;}
  Serial.print(psiVal);
  Serial.println();

  //PIS_AngleVal=50;
  PIS_AngleVal = (int)psiVal;

  //offset a1 and a2 from angle to give array coordinates of bottom of triangle
  markerOffset1=PIS_AngleVal-8;
  markerOffset2=PIS_AngleVal+8;

  //if we get within offest range of 0 or 100 limit the offset
  if(PIS_AngleVal < 8) {markerOffset1=1;}
  if(PIS_AngleVal > 92) {markerOffset2=99;}
  
  digitalWrite(CS_psi, LOW);
  // put your main code here, to run repeatedly:
  dialFace.drawString(String(PIS_AngleVal),120,114);
  if (psiVal < 70) {
    //Paint the dial needle
    dialFace.fillTriangle(x_psi[PIS_AngleVal],y_psi[PIS_AngleVal],x2_psi[markerOffset1],y2_psi[markerOffset1],x2_psi[markerOffset2],y2_psi[markerOffset2],TFT_BLUE);
  } else {
    //Paint the dial needle
    dialFace.fillTriangle(x_psi[PIS_AngleVal],y_psi[PIS_AngleVal],x2_psi[markerOffset1],y2_psi[markerOffset1],x2_psi[markerOffset2],y2_psi[markerOffset2],TFT_RED);
  }
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_psi, HIGH);

}



void ProcessTEMP() {
  

  digitalWrite(CS_temp, LOW);
  //push fresh copy of image to remove last needle and prevent strobing
  dialFace.pushImage(0,0,240,240,Dial_temp_120);
  digitalWrite(CS_temp, HIGH);

  uint8_t i;                                          //integer for loop
  float average;                                      //decimal for average

  for (i=0; i<NUMSAMPLES; i++) {                      
    samples[i] = analogRead(GPIO_ERG);        //takes samples at number defined with a short delay between samples
    delay(10);
  }

  sensorTemp = 0;
  for (i=0; i< NUMSAMPLES; i++) {
    sensorTemp += samples[i];                            //adds all number of samples together
  }

  
  sensorTempAveraged = sensorTemp / NUMSAMPLES;                              //divides by number of samples to output the average
  sensorTempVoltage = (coolantsensorDivider*sensorTempAveraged)/(4095-sensorTempAveraged);        //conversion equation to read resistance from sensorPsiVoltage divider
  
  float steinhart;                              //steinhart equation to estimate temperature value at any resistance from curve of thermistor sensor
  steinhart = log(sensorTempVoltage);                     //lnR
  steinhart = pow(steinhart,3);                 //(lnR)^3
  steinhart *= steinconstC;                     //C*((lnR)^3)
  steinhart += (steinconstB*(log(sensorTempVoltage)));    //B*(lnR) + C*((lnR)^3)
  steinhart += steinconstA;                     //Complete equation, 1/T=A+BlnR+C(lnR)^3
  steinhart = 1.0/steinhart;                    //Inverse to isolate for T
  tempVal = steinhart - 273.15;                          //Conversion from kelvin to celcius



  Serial.print(tempVal);
  Serial.println();



  //catch all for negative psi values
  if(tempVal < 0) {tempVal=0;}
  //catch all for negative psi values
  if(tempVal > 120) {tempVal=120;}
  //Serial.print(psiVal);
  //Serial.println();

  TEMP_AngleVal=120;
  //TEMP_AngleVal = (int)tempVal;//+ random(1,20);


  //offset a1 and a2 from angle to give array coordinates of bottom of triangle
  tempMarkerOffset1=TEMP_AngleVal-8;
  tempMarkerOffset2=TEMP_AngleVal+8;

  //if we get within offest range of 0 or 100 limit the offset
  if(TEMP_AngleVal < 8) {tempMarkerOffset1=1;}
  if(TEMP_AngleVal > 112) {tempMarkerOffset2=119;}

  digitalWrite(CS_temp, LOW);
  // put your main code here, to run repeatedly:
  dialFace.drawString(String(TEMP_AngleVal),120,114);
  if (TEMP_AngleVal < 70) {
    //Paint the dial needle
    dialFace.fillTriangle(x_temp[TEMP_AngleVal],y_temp[TEMP_AngleVal],x2_temp[tempMarkerOffset1],y2_temp[tempMarkerOffset1],x2_temp[tempMarkerOffset2],y2_temp[tempMarkerOffset2],TFT_BLUE);
  } else {
    //Paint the dial needle
    dialFace.fillTriangle(x_temp[TEMP_AngleVal],y_temp[TEMP_AngleVal],x2_temp[tempMarkerOffset1],y2_temp[tempMarkerOffset1],x2_temp[tempMarkerOffset2],y2_temp[tempMarkerOffset2],TFT_RED);
  }
  dialFace.pushSprite(0, 0);
  digitalWrite(CS_temp, HIGH);

  Serial.print("Temperature = ");
  Serial.print(TEMP_AngleVal);                      //prints final temp in celcius
  Serial.println(" *C");
  




}

void SetupPSITickArray(float refVal) {

  float tickRef = refVal;

  for (int i = 0; i < 101; i++) {
    // the angle is always radians as thats what cos & sine need so conver degrees to radians with rad
    x_psi[i] = OuterRadius * cos(tickRef * rad) + xoffset;
    y_psi[i] = OuterRadius * sin(tickRef * rad) + yoffset;

    x2_psi[i] = InnerRadius * cos(tickRef * rad) + xoffset;
    y2_psi[i] = InnerRadius * sin(tickRef * rad) + yoffset;

    //if(angle=5.759586532) dialFace.drawPixel(120, 120, TFT_RED);
    //increment the andle by the radian 
    tickRef += Dial100_AngleIncrement;
    //reset
    if(tickRef>358.80) tickRef=1.2;
  }
}

void SetupTEMPTickArray(float refVal) {

  float tickRef = refVal;

  for (int i = 0; i < 121; i++) {
    // the angle is always radians as thats what cos & sine need so conver degrees to radians with rad
    x_temp[i] = OuterRadius * cos(tickRef * rad) + xoffset;
    y_temp[i] = OuterRadius * sin(tickRef * rad) + yoffset;

    x2_temp[i] = InnerRadius * cos(tickRef * rad) + xoffset;
    y2_temp[i] = InnerRadius * sin(tickRef * rad) + yoffset;

    //if(angle=5.759586532) dialFace.drawPixel(120, 120, TFT_RED);
    //increment the andle by the radian 
    tickRef += Dial120_AngleIncrement;
    //reset
    if(tickRef>358.80) tickRef=1.2;
  }
}

void SetupERGTickArray(float refVal) {

  float tickRef = refVal;

  for (int i = 0; i < 101; i++) {
    // the angle is always radians as thats what cos & sine need so conver degrees to radians with rad
    x_erg[i] = OuterRadius * cos(tickRef * rad) + xoffset;
    y_erg[i] = OuterRadius * sin(tickRef * rad) + yoffset;

    x2_erg[i] = InnerRadius * cos(tickRef * rad) + xoffset;
    y2_erg[i] = InnerRadius * sin(tickRef * rad) + yoffset;

    //if(angle=5.759586532) dialFace.drawPixel(120, 120, TFT_RED);
    //increment the andle by the radian 
    tickRef += Dial100_AngleIncrement;
    //reset
    if(tickRef>358.80) tickRef=1.2;
  }
}

uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

uint32_t readADC_Avg(int ADC_Raw)
{
  int i = 0;
  uint32_t Sum = 0;
  
  AN_Pot1_Buffer[AN_Pot1_i++] = ADC_Raw;
  if(AN_Pot1_i == FILTER_LEN)
  {
    AN_Pot1_i = 0;
  }
  for(i=0; i<FILTER_LEN; i++)
  {
    Sum += AN_Pot1_Buffer[i];
  }
  return (Sum/FILTER_LEN);
}