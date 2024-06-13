
#include <cmath>
#include <TFT_eSPI.h> 
#include "Dial.h"

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite ln = TFT_eSprite(&tft);


const int numSegments = 150;
const int radius = 100;


void setup() {
  // Initialize Arduino setup code here

  Serial.begin(9600); 

  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  img.setColorDepth(8);
  img.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  img.createSprite(240, 240);

  tft.setPivot(60,60);
  img.setTextDatum(4);
  img.setTextColor(TFT_BLACK,0xAD55);
  img.pushImage(0,0,240,240,Dial);
  

  //start at zero and  work anti clockwise
  //float angle=5.759586532; //330 degrees
  float angle=330; //330 degrees
  double rad = PI / 180;

  //angle increment is returned in radians (multiple by 180/PI to get degrees)
  float angleIncrement = 2 * PI / numSegments;
  //so above works out as (2 * 3.1415) / 360 = 0.01745329 radian
  //0.01745329 radian expressed in degrees is 0.01745329 * (180/3.1415) = 1




  // so the below loop is taking an angle in degrees using the counter i
  // then adding on the andle increment whcih is in radians
  // then taking the cos & sin * radius to get the x & y of the coordinate
  for (int i = 0; i < 101; i++) {

    // the angle is always radians as thats what cos & sine need so conver degrees to radians with rad
    float x = radius * cos(angle * rad);
    float y = radius * sin(angle * rad);

    //if(angle=5.759586532) img.drawPixel(120, 120, TFT_RED);
  
    Serial.print(i);
    Serial.print("\t");  
    Serial.print(angle);
    Serial.print("\t");  
    Serial.print(angleIncrement);
    Serial.print("\t");  
    Serial.print(x);  
    Serial.print("\t");  
    Serial.print(y);  
    Serial.println();  

    //increment the andle by the radian 
    //angle += angleIncrement;
    angle += 2.4;

    img.drawPixel(x+120, y+120, TFT_WHITE);

    img.pushSprite(0, 0);
    delay(1000);

        //reset
    if(angle>358.80) angle=1.2;
  }
}

void loop() {

  // Any other code or delay for the loop
}
