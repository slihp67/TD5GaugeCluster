#include <TFT_eSPI.h> 

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite ln = TFT_eSprite(&tft);


int angle;
int sx=120;
int sy=120;
int r1=100;
int r2=40;

double rad=0.01745;
float x[360];
float y[360];
float x2[360];
float y2[360];

double rad24=0.04189;
float x100[150];
float y100[150];
float x2100[150];
float y2100[150];


const int numSegments = 150;
const int radius = 100;

float angle=0;
float angleIncrement = 2 * PI / numSegments;

void setup() {



    Serial.begin(9600); 
    
    tft.init();
    tft.setRotation(2);
    tft.setSwapBytes(true);
    img.setColorDepth(8);
    img.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    img.createSprite(240, 240);
  
    tft.setPivot(60,60);
    img.setTextDatum(4);
    img.setTextColor(TFT_BLACK,0xAD55);
    

    //########################
    //we are still going round a 360 degree circle but we are doing it in 2.4 increments 
    //which should give us 150 points of refernce in the circle.
    int i2=0;
    float a2=137;

    while(a2!=44)
    {
    x100[i2]=r1*cos(a2)+sx;
    y100[i2]=r1*sin(a2)+sy;

    Serial.print(a2);  // prints a label
    Serial.print("\t");  
    Serial.print(i2);  // prints a label
    Serial.print("\t");  
    Serial.print(x100[i2]);  // prints a label
    Serial.print("\t");  
    Serial.print(y100[i2]);  // prints a label
    Serial.println();  
    img.pushSprite(0, 0);
    delay(500);

    x2100[i2]=(r2)*cos(a2)+sx;
    y2100[i2]=(r2)*sin(a2)+sy;
    img.drawPixel(x100[i2], y100[i2], TFT_RED);
    img.drawPixel(x2100[i2], y2100[i2], TFT_BLUE);

    
    //increment the angle by 2.4 degrees
    a2 += 2.4;
    //increment the counter by 1
    i2++;
    //reset
    if(a2==360) a2=0;

 
   
    }
  //#########################
    /*
    int a1,a2;
    int angle;
    angle=50;    

    a1=angle-8;
    a2=angle+8;

    if(a1<0) a1=angle-4;
    if(a2>=359) a2=angle+4;

    img.fillTriangle(x100[angle],y100[angle],x2100[angle],y2100[angle],x2100[a2+2],y2100[a2+2],TFT_RED);
*/
    img.pushSprite(0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
