#include <TFT_eSPI.h> 

TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite ln = TFT_eSprite(&tft);

double rad=0.01745;
int angle;

int sx=120;
int sy=120;
int r1=100;
int r2=40;


float x[360];
float y[360];
float x2[360];
float y2[360];

void setup() {

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
    

    int i=0;
    int a=137;

    while(a!=44)
    {
    x[i]=r1*cos(rad*a)+sx;
    y[i]=r1*sin(rad*a)+sy;
    x2[i]=(r2-20)*cos(rad*a)+sx;
    y2[i]=(r2-20)*sin(rad*a)+sy;
    img.drawPixel(x[i], y[i], TFT_WHITE);
    img.drawPixel(x2[i], y2[i], TFT_WHITE);

    
    //increment
    i++;
    a++;
    //reset
    if(a==360) a=0;
   
    }
    
    int a1,a2;
    int angle;
    angle=130;    

    a1=angle-8;
    a2=angle+8;

    a1=angle-4;
    a2=angle+4;

 
    drawWideLine(x[angle],y[angle],x2[angle],y2[angle],TFT_RED,10);

    img.pushSprite(0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

}



void drawWideLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, uint8_t width) {
  if (x1 == x2) {
    // Draw a vertical line
    for (int i = 0; i < width; i++) {
      int16_t x = x1 - (width / 2) + i;
      tft.drawFastVLine(x, y1, abs(y2 - y1) + 1, color);
    }
  } else if (y1 == y2) {
    // Draw a horizontal line
    for (int i = 0; i < width; i++) {
      int16_t y = y1 - (width / 2) + i;
      tft.drawFastHLine(x1, y, abs(x2 - x1) + 1, color);
    }
  } else {
    // Draw a diagonal line
    float slope = ((float)(y2 - y1)) / ((float)(x2 - x1));
    float angle = atan(slope);
    float offset = (width / 2) * sin(angle);

    for (int i = 0; i < width; i++) {
      int16_t x_offset = (i - (width / 2)) * sin(angle);
      int16_t y_offset = (i - (width / 2)) * cos(angle);

      int16_t x_start = x1 + x_offset - offset;
      int16_t y_start = y1 + y_offset + offset;
      int16_t x_end = x2 + x_offset - offset;
      int16_t y_end = y2 + y_offset + offset;

      img.drawLine(x_start, y_start, x_end, y_end, color);
    }
  }
}

