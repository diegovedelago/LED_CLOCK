#include "timeunit_class.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN 4

// How many NeoPixels are attached to the Arduino?
// Defined in timeunit_class.h

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
//SDA pin 2, SCL pin 3
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

RTC_DS1307 rtc;
DateTime now;

//iniztialization of timeUnit objects
LedStripRunner secondi;
LedStripRunner minuti;
LedStripRunner ore;

//definition of RGB array to store each pixel's r/g/b amount
uint32_t arrayColorR[NUMPIXELS];
uint32_t arrayColorG[NUMPIXELS];
uint32_t arrayColorB[NUMPIXELS];

uint8_t htempColorRED =0; //= (uint8_t)((secondi.red + minuti.red + ore.red)/3);         //trying to equal out the various 
uint8_t htempColorGREEN =0; //= (uint8_t)((secondi.green + minuti.green + ore.green)/3); //components before the sum
uint8_t htempColorBLUE =0; //= (uint8_t)((secondi.blue + minuti.blue + ore.blue)/3);

uint8_t mtempColorRED =0;
uint8_t mtempColorGREEN =0;
uint8_t mtempColorBLUE =0;

uint8_t stempColorRED =0;
uint8_t stempColorGREEN =0;
uint8_t stempColorBLUE =0;

void setup () {

  Serial.begin(9600);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  pixels.begin(); // This initializes the NeoPixel library.

  //definition of "time" objects (speedUP/DOWN & color constructor)
  secondi = LedStripRunner(0.005, 0.0015, 20, 5, 60);
  minuti = LedStripRunner(0.01, 0.006, 5, 30, 80);
  ore = LedStripRunner(0.01, 0.006, 12, 3, 40);

}


void loop() {
  
  pixels.clear();
  now = rtc.now();

  //updating intensity for each unit; see "fading method".
  secondi.UpdateIntensity(now.second());
  minuti.UpdateIntensity(now.minute());
  ore.UpdateIntensity(hourposition(now.hour()));

  for(int i=0; i<NUMPIXELS; i++){
    pixels.setPixelColor(i, pixels.Color(ore.intensity[i], ore.intensity[i], ore.intensity[i]));
    
    pixels.setPixelColor(i, pixels.Color(minuti.intensity[i], minuti.intensity[i], minuti.intensity[i]));
    
    pixels.setPixelColor(i, pixels.Color(secondi.intensity[i], secondi.intensity[i], secondi.intensity[i]));
    
  }

  pixels.show(); //upload data to WS2812
  
}



int hourposition(int rtchour) {     //position of hour indicator led on clock's circumference
  if(rtchour >= 12){
    rtchour-=12;
  }
  if(rtchour==0)
    return 0;
      
  rtchour *=5;
  rtchour-=1;
  
  return rtchour;
}

//void unitPixelSet (int pos, LedStripRunner unit) {    //function that set a pixel 
//  pixels.setPixelColor(pos, pixels.Color(unit.intensity*unit.red, unit.intensity*unit.green, unit.intensity*unit.blue));
//}
