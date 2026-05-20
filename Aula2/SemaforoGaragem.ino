#include <Adafruit_NeoPixel.h>

#define PIN 8
#define NUMPIXELS 16

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
}

void loop() {
   pixels.clear();               
   pixels.show();
   delay(1000)
   pixels.setPixelColor(0, pixels.Color(255, 255, 0));  
   pixels.show();   
   delay(1000);  
 }