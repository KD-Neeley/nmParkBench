/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/kdneeley/Documents/IoT/nmParkBench/NMParkBench/src/NMParkBench.ino"
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
#line 23 "/Users/kdneeley/Documents/IoT/nmParkBench/NMParkBench/src/NMParkBench.ino"
const int MSGEQ7_STROBE_PIN =    7;
const int MSGEQ7_RESET_PIN =     8;
const int MSGEQ7_ANALOG_PIN =   16;
const int NUM_FREQUENCY_BANDS =  7;

//Globals for the MSGEQ7
int volumeInBand[NUM_FREQUENCY_BANDS];
int minimumVolume, maximumVolume;
int band1, band2, band3, band4, band5, band6, band7;

//PINS and Constants for the Lights
const int NLEDS = 324; // Number of LEDs
const int CLKPIN = 14; //Clock In Pin
const int DPIN = 15; //Data In Pin
const int MULTIPLE = 4;
const int OFFSETDELTA = 2;

//variables and constants for the motion sensor LIDAR LITE V4
const int I2CADDRESS = 0X62;
uint16_t distance;
int threshold = 5000;

//Globals for the lights LPD8806
int numArray[NLEDS];
int offset = 0;
int displayLight;

//declare object for the lights
LPD8806 lights = LPD8806(NLEDS, CLKPIN, DPIN);
//declare the object for the motion sensor
LIDARLite_v4LED L1;

//functions
int pixelFill (int startPixel, int endPixel, int green, int red, int blue); 
int maxRead(int band);
void transitionColor(int pixel, int red, int green, int blue); 

void setup() {
  Serial.begin(115200);
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
  // L1.getBusyFlag(I2CADDRESS);
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
    //purple 0x0022FF GRB
    transitionColor(0, 0x22, 0x00, 0xFF);
  }
  if(displayLight == band2) {
    //blue 0x0000FF GRB
    transitionColor(0, 0x00, 0x00, 0xFF);
  }
  if(displayLight == band3) {
    //green 0xFF0000
    transitionColor(0, 0x00, 0xFF, 0x00);
  }
  if(displayLight == band4) {
    //yellow 0xFFFF00 GRB
    transitionColor(0, 0xFF, 0xFF, 0x00);
  }
  if(displayLight == band5) {
    //orange 0x33FF00 GRB
    transitionColor(0, 0xFF, 0x33, 0x00);
  }
  if(displayLight == band6) {
    //magenta 0x00FFFF GRB
    transitionColor(0, 0xFF, 0x00, 0xFF);
  }
  if(displayLight == band7) {
    //red 0x00FF00 GRB
    transitionColor(0, 0xFF, 0x00, 0x00);
  }
}
else {
  displayLight = pixelFill(0, NLEDS-1, 0x00, 0x00, 0x00);
}
}

int pixelFill (int startPixel, int endPixel, int green, int red, int blue) {

  for(int i=startPixel; i<endPixel-1; i++) {
    lights.setPixelColor(i, green, red, blue);
  }
  lights.show();
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

//Thanks to Dr. Brian Rashap for providing the following function
// Brian's smooth transition to a new color
void transitionColor(int pixel, int red, int green, int blue) {
    int oldRed, oldGreen, oldBlue, oldColor;
    int newRed, newGreen, newBlue;

    oldColor = lights.getPixelColor(0);
    oldRed = oldColor >>16;
    oldGreen = oldColor >> 8 | 0x0000FF;
    oldBlue = oldColor | 0x0000FF;

    newRed = oldRed + (red - oldRed)/5;
    newGreen = oldGreen + (green - oldGreen)/5;
    newBlue = oldBlue + (blue - oldBlue)/5;

    // lights.setPixelColor(pixel, red, green, blue);
    lights.setPixelColor(0, green, red, blue);
    pixelFill(0, NLEDS-1, green, red, blue);
    lights.show();
}