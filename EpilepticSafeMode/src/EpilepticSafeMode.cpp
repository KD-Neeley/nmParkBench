/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/kdneeley/Documents/IoT/nmParkBench/EpilepticSafeMode/src/EpilepticSafeMode.ino"
/*
 * Project NMParkBench
 * Description: Program to run the NM Park Bench, a solar powered park bench with motion sensors that turn it on, wireless phone charging, solar powered, with LEDs that light up the seat in response to sound.
              * COMPONENT LIST:
                    * Solar Panel
                    * 12V Battery
                    * wireless phone chargers x4
                    * 12V down to 5V step-down buck converters x2
                    * MSGEQ7 sound sensor 
                    * GARMIN LIDAR LITE V4 motion sensors x2
                    * LPD8806 weatherproof strip of 162 RGB LEDs x2
 * 
 * Author: KD Neeley
 * Date: 06/17/2023
 */ 
#include <LPD8806.h> 
#include <SPI.h>
#include "kdsRainbows.h"
#include <LIDARLite_v4LED.h>
#include <wire.h>

//Pins for the MSGEQ7
void setup();
void loop();
#line 23 "/Users/kdneeley/Documents/IoT/nmParkBench/EpilepticSafeMode/src/EpilepticSafeMode.ino"
const int MSGEQ7_STROBE_PIN =    7;
const int MSGEQ7_RESET_PIN =     8;
const int MSGEQ7_ANALOG_PIN =   16;
const int NUM_FREQUENCY_BANDS =  7;

//Globals for the MSGEQ7
int volumeInBand[NUM_FREQUENCY_BANDS];
int minimumVolume, maximumVolume;
int band1, band2, band3, band4, band5, band6, band7;

//PINS and Constants for the Lights
const int NLEDS = 162; // Number of LEDs
const int CLKPIN = 14; //Clock In Pin
const int DPIN = 15; //Data In Pin
const int MULTIPLE = 4;
const int OFFSETDELTA = 20;

//variables and constants for the motion sensor LIDAR LITE V4
const int I2CADDRESS = 0X62;
uint16_t distance;
int threshold = 800;

//Globals for the lights LPD8806
int numArray[NLEDS];
int offset = 0;
int displayLight;

//declare object for the lights
LPD8806 lights = LPD8806(NLEDS, CLKPIN, DPIN);
//declare the object for the motion sensor
LIDARLite_v4LED L1;

//functions
int pixelFill (int startPixel, int endPixel, int hexColor); 
int maxRead(int band);
void cycle(unsigned int offset, unsigned int s, unsigned int v);
uint32_t hsvToColour(unsigned int h, unsigned int s, unsigned int v);
void launchRainbow();




SYSTEM_MODE(SEMI_AUTOMATIC);



void setup() {
  Serial.begin(115200);
  waitFor(Serial.isConnected, 10000);
  Wire.begin();

   // Set up the MSGEQ7 IC
  pinMode(MSGEQ7_ANALOG_PIN, INPUT);
  pinMode(MSGEQ7_STROBE_PIN, OUTPUT);
  pinMode(MSGEQ7_RESET_PIN, OUTPUT);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  digitalWrite(MSGEQ7_STROBE_PIN, HIGH);

  // Initialize the minimum and maximum volume levels
  minimumVolume = 1023;
  maximumVolume = 0;

  //setup the lights
  lights.begin();
  lights.show();
}

void loop() {
  L1.takeRange(I2CADDRESS);
  L1.waitForBusy(I2CADDRESS);
  L1.getBusyFlag(I2CADDRESS);
  distance = L1.readDistance(I2CADDRESS);
  Serial.printf("Distance = %u\n", distance);

if(distance<threshold) {
  // Toggle the RESET pin of the MSGEQ7 to start reading from the lowest frequency band
  digitalWrite(MSGEQ7_RESET_PIN, HIGH);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);

  // Read the volume in every frequency band from the MSGEQ7
  //save the reading to each respective band
  for (int i=0; i<NUM_FREQUENCY_BANDS; i++) {
    digitalWrite(MSGEQ7_STROBE_PIN, LOW);
    delayMicroseconds(30); // Allow the output to settle
    volumeInBand[i] = analogRead(MSGEQ7_ANALOG_PIN);
    digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
    band1 = volumeInBand[0];
    band2 = volumeInBand[1];
    band3 = volumeInBand[2];
    band4 = volumeInBand[3];
    band5 = volumeInBand[4];
    band6 = volumeInBand[5];
    band7 = volumeInBand[6];
    displayLight = maxRead(band1);
  }

  Serial.printf("Band1 = %i\n", band1);
  Serial.printf("Band2 = %i\n", band2);
  Serial.printf("Band3 = %i\n", band3);
  Serial.printf("Band4 = %i\n", band4);
  Serial.printf("Band5 = %i\n", band5);
  Serial.printf("Band6 = %i\n", band6);
  Serial.printf("Band7 = %i\n", band7);

  if(displayLight == band1) {
    launchRainbow();
  }
   if(displayLight == band2) {
    launchRainbow();
  }
   if(displayLight == band3) {
    launchRainbow();
  }
   if(displayLight == band4) {
    launchRainbow();
  }
   if(displayLight == band5) {
    launchRainbow();
  }
   if(displayLight == band6) {
    launchRainbow();
  }
   if(displayLight == band7) {
    launchRainbow();
  }

} //if distance < threshold

else { 
  displayLight = pixelFill(0, NLEDS-1, nada);
  Serial.printf("Nada\n");
}
}

void launchRainbow(){
       unsigned int n, mode;
     for (mode = 0; mode < 8; mode++) {
        for (n = 0; n < 256; n+=5) {
          switch (mode) {
            case 0: // Fade from black to full saturated colour
              cycle(offset, 255, n);
              break;
            case 5: // Fade from full colour to white
              cycle(offset, 255-n, 255);
              break;
            case 6: // Fade from white to full colour
              cycle(offset, n, 255);
              break;
            case 7: // Fade from full colour to black
              cycle(offset, 255, 255-n);
              break;
            default: // Cycle with full saturation and value
              cycle(offset, 255, 255);
          }
        }   
        // Increment the offset to animate the colour pattern
        offset = (offset + OFFSETDELTA) % lights.numPixels();
      }
}
    



int pixelFill (int startPixel, int endPixel, int hexColor) {

for(int i=startPixel; i<endPixel-1; i++) {
  lights.setPixelColor(i, hexColor);
}
 lights.show();
//  delay(500);
 return(endPixel);
}

int maxRead(int band) {
  int maxReading = band1;
    if(maxReading<band2); {
      maxReading = band2;
    }
    if(maxReading<band3) {
      maxReading =band3;
    }
    if(maxReading<band4) {
      maxReading = band4;
    }
    if(maxReading<band5) {
      maxReading = band5;
    }
    if(maxReading<band6) {
      maxReading = band6;
    }
    if(maxReading<band7) {
      maxReading = band7;
    }
    return maxReading;
}

// Set LED colours into a rainbow with specified saturation and value
void cycle(unsigned int offset, unsigned int s, unsigned int v) {
  unsigned int n;
  for (n = 0; n < lights.numPixels(); n++)
    lights.setPixelColor(n, hsvToColour(n * MULTIPLE + offset,s,v)); 
  lights.show();
  delay(100);
}
uint32_t hsvToColour(unsigned int h, unsigned int s, unsigned int v) {

  unsigned char region, remainder, p, q, t;

  // Sanity check ranges and check for no saturation
  h = h % 256;
  if (s > 255) s = 255;
  if (v > 255) v = 255;
  else v = (v * v) >> 8;
  if (s == 0) return lights.Color(v >> 1, v >> 1, v >> 1);

  // Map HSV to RGB, use to build a colour value for the strip library
  region = h / 43;
  remainder = (h - (region * 43)) * 6; 
  p = (v * (255 - s)) >> 9;
  q = (v * (255 - ((s * remainder) >> 8))) >> 9;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 9;
  v = v >> 1;
  switch (region) {
  case 0:
    return lights.Color(v, p, t);
  case 1:
    return lights.Color(q, p, v);
  case 2:
    return lights.Color(p, t, v);
  case 3:
    return lights.Color(p, v, q);
  case 4:
    return lights.Color(t, v, p);
  }
  return lights.Color(v, q, p);
}
