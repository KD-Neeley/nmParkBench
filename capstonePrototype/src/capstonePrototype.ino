/*
 * Project capstonePrototype
 * Description: Re-writing my code thanks to Murphy.
 *              The Solar Powered Park Bench will provide visitors with the ability to
 *              charge their devices using a USB port. The lights on the bench will
 *              provide mild entertainment by responding to sounds in the environment.
 * 
 *              * Get sound detection using the MSGEQ7 sound detector 
 *              Make NeoPixels respond to the sound input
 *              Detect the presence of someone sitting on the bench 
 *              Put the program in sleep mode when no presence is detected
 *              Wake the program and run it when a presence is detected
 *              * Power the microcontroller using a Solar Panel 
 *              Charge a 12v battery using the Solar Panel
 *              Switch power to battery operation when the sun is gone
 *              * Provide a charge to a USB port to charge common devices (Phones, Ipads, etc)
 *              * Get the train and bus schedules for the installation site
 *              * Display the train and bus schedules on the OLED
 *              * Display the current time and date on the OLED
 *              * Display the current temperature on the OLED
 *              * Publish the data to a dashboard
 * 
 * Author: Katie Neeley
 * Date: 04/21/2023
 */

#include <neopixel.h>
#include "kdsRainbows.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <JsonParserGeneratorRK.h>
#include "credentials.h"

//GLOBALS
//MSGEQ7 Sound Sensor
int const STROBE = A3; 
int const OUT = A4;
int const RESETPIN = A2;
int soundInput;
int soundLevel[7];
int pulse;

//Neopixels
int const PIXELCOUNT= 16;
int const PIXELPIN = D8;
int startPixel;
int endPixel;
int hexColor;
int brightness;

//OLED
const int OLED_RESET = D4;

//Distance Sensor
const int MOTIONSENSOR = A1;
int distance;
int distanceThreshold = 2900;

//QR Code for Dashboard
// 'QRcode', 128x64px
// 'QRcode', 128x64px
const unsigned char QRcode [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x60, 0x78, 0x18, 0x06, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x60, 0x78, 0x18, 0x06, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x66, 0x61, 0xe0, 0x66, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x66, 0x61, 0xe0, 0x66, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x7e, 0x67, 0xe7, 0xfe, 0x60, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x7e, 0x67, 0xe7, 0xfe, 0x60, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x67, 0x9e, 0x19, 0xfe, 0x60, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x67, 0x9e, 0x19, 0xfe, 0x60, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x79, 0xfe, 0x61, 0x9e, 0x60, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x79, 0xfe, 0x61, 0x9e, 0x60, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x7f, 0x99, 0x86, 0x1e, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x7f, 0x99, 0x86, 0x1e, 0x7f, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x66, 0x66, 0x66, 0x66, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x66, 0x66, 0x66, 0x66, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xe7, 0x80, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xe7, 0x80, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x18, 0x19, 0xf9, 0x98, 0x1e, 0x79, 0x87, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x18, 0x19, 0xf9, 0x98, 0x1e, 0x79, 0x87, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x61, 0x87, 0xf8, 0x1e, 0x00, 0x07, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x61, 0x87, 0xf8, 0x1e, 0x00, 0x07, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x18, 0x1e, 0x61, 0xe1, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x18, 0x1e, 0x61, 0xe1, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xe7, 0xff, 0xe6, 0x07, 0x9f, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xe7, 0xff, 0xe6, 0x07, 0x9f, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x66, 0x79, 0x86, 0x19, 0xe7, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x66, 0x79, 0x86, 0x19, 0xe7, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf9, 0xe1, 0xe0, 0x66, 0x60, 0x1e, 0x7e, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf9, 0xe1, 0xe0, 0x66, 0x60, 0x1e, 0x7e, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x66, 0x7f, 0x81, 0x87, 0x98, 0x1e, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x66, 0x7f, 0x81, 0x87, 0x98, 0x1e, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf9, 0xe7, 0x81, 0x86, 0x66, 0x1f, 0x87, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf9, 0xe7, 0x81, 0x86, 0x66, 0x1f, 0x87, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf8, 0x1e, 0x61, 0x86, 0x1e, 0x61, 0xe1, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf8, 0x1e, 0x61, 0x86, 0x1e, 0x61, 0xe1, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf9, 0xf9, 0x99, 0xfe, 0x66, 0x67, 0x98, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf9, 0xf9, 0x99, 0xfe, 0x66, 0x67, 0x98, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x00, 0x00, 0x06, 0x7e, 0x66, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x00, 0x00, 0x06, 0x7e, 0x66, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x67, 0xff, 0x80, 0x01, 0x9e, 0x66, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x67, 0xff, 0x80, 0x01, 0x9e, 0x66, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf8, 0x60, 0x60, 0x18, 0x00, 0x60, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf8, 0x60, 0x60, 0x18, 0x00, 0x60, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x67, 0xff, 0xe7, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x67, 0xff, 0xe7, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x67, 0xe6, 0x67, 0x86, 0x61, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x67, 0xe6, 0x67, 0x86, 0x61, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x66, 0x18, 0x1e, 0x07, 0xe1, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x66, 0x18, 0x1e, 0x07, 0xe1, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x78, 0x7f, 0xf8, 0x60, 0x06, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x78, 0x7f, 0xf8, 0x60, 0x06, 0x1f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x61, 0xe7, 0x87, 0xe1, 0x81, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x61, 0xe7, 0x87, 0xe1, 0x81, 0xe7, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x61, 0x99, 0xf8, 0x07, 0x9e, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe6, 0x06, 0x61, 0x99, 0xf8, 0x07, 0x9e, 0x67, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x79, 0x80, 0x1e, 0x61, 0x99, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe7, 0xfe, 0x79, 0x80, 0x1e, 0x61, 0x99, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x66, 0x7f, 0x9e, 0x00, 0x7f, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x66, 0x7f, 0x9e, 0x00, 0x7f, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//WEBHOOK GLOBALS
const char *EVENT_NAME = "GetWeatherData";
unsigned int lastTime;
const float lat=35.0045, lon=-106.6465;

//function prototype
int pixelFill (int startPixel, int endPixel, int brightness, int hexColor);

//Objects
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, WS2812B);
Adafruit_SSD1306 myDisplay(OLED_RESET);

SYSTEM_MODE(AUTOMATIC); //NOTE: AUTOMATIC NEVER CONNECTS TO WIFI
// SYSTEM_MODE(SEMI_AUTOMATIC);



void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 15000);
  String subscriptionName = String::format("%s/%s/", System.deviceID().c_str(), EVENT_NAME);
  Particle.subscribe(subscriptionName, subscriptionHandler, MY_DEVICES);
  Serial.printf("Subscribing to %s\n", subscriptionName.c_str());
  pixel.begin();
 
  //NOTE: COMMENTING OUT THE BELOW ALLOWS US TO CONENCT TO WIFI, WITH IT THE ARGON NEVER CONNECTS.
  //Connect to Internet but not Particle Cloud
  // WiFi.on();
  // WiFi.connect(); 
  // while(WiFi.connecting()) {
  //   Serial.printf(".");
  // }
  // Serial.printf("\n\n");

  //initialize Sound Input
  // Define our pin modes
  pinMode(STROBE, OUTPUT);
  pinMode(RESETPIN, OUTPUT);
  pinMode(OUT, INPUT);
 
  // Initialize the Sound Sensor Pins
  digitalWrite(RESETPIN, LOW);
  digitalWrite(STROBE, LOW);
  delay(1);
 
  // Reset MSGEQ7 
  digitalWrite(RESETPIN, HIGH);
  delayMicroseconds(.1);
  digitalWrite(RESETPIN, LOW);
  digitalWrite(STROBE, HIGH); 
  delayMicroseconds(72);

  //Initialize the OLED
  myDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  myDisplay.clearDisplay();

  //Initialize the IR Distance Sensor
  pinMode(MOTIONSENSOR, INPUT);
}

void loop() {
  distance = analogRead(MOTIONSENSOR); //Get Distance to determine if the program will run given a presence
  Serial.printf("Distance Read is %i\n", distance);

  if(distance > distanceThreshold) { //if a presence is sensed run the program
    //GET Sound
        // Cycle through each frequency band by pulsing the strobe.
        //Sound Level 0
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[0] = analogRead(OUT);
        Serial.printf("Level 0\n Value %i\n", soundLevel[0]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[0] < 400) {
            pixelFill(0, 2, 50, fullred);
        }
        if(soundLevel[0] >= 400 && soundLevel[0] < 1000) {
            pixelFill(0, 2, 100, fullred);
        }
        if(soundLevel[0] >= 1000 && soundLevel[0] < 2000) {
            pixelFill(0, 2, 150, fullred);
        }
        if(soundLevel[0] >= 2000 && soundLevel[0] < 3000) {
            pixelFill(0, 2, 200, fullred);
        }
        if(soundLevel[0] >= 3000) {
            pixelFill(0, 2, 255, fullred);
        }

        //Sound Level 1
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[1] = analogRead(OUT);
        Serial.printf("Level 1\n Value %i\n", soundLevel[1]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[1] < 400) {
            pixelFill(3, 5, 50, fullmagenta);
        }
        if(soundLevel[1] >= 400 && soundLevel[1] < 1000) {
            pixelFill(3, 5, 100, fullmagenta);
        }
        if(soundLevel[1] >= 1000 && soundLevel[1] < 2000) {
            pixelFill(3, 5, 150, fullmagenta);
        }
        if(soundLevel[1] >= 2000 && soundLevel[1] < 3000) {
            pixelFill(3, 5, 200, fullmagenta);
        }
        if(soundLevel[1] >= 3000) {
            pixelFill(3, 5, 255, fullmagenta);
        }
         //Sound Level 2
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[2] = analogRead(OUT);
        Serial.printf("Level 2\n Value %i\n", soundLevel[2]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[2] < 400) {
            pixelFill(6, 8, 50, purple);
        }
        if(soundLevel[2] >= 400 && soundLevel[2] < 1000) {
            pixelFill(6, 8, 100, purple);
        }
        if(soundLevel[2] >= 1000 && soundLevel[2] < 2000) {
            pixelFill(6, 8, 150, purple);
        }
        if(soundLevel[2] >= 2000 && soundLevel[2] < 3000) {
            pixelFill(6, 8, 200, purple);
        }
        if(soundLevel[2] >= 3000) {
            pixelFill(6, 8, 255, purple);
        }
        //Sound Level 3
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[3] = analogRead(OUT);
        Serial.printf("Level 3\n Value %i\n", soundLevel[3]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[3] < 400) {
            pixelFill(9, 11, 50, fullblue);
        }
        if(soundLevel[3] >= 400 && soundLevel[3] < 1000) {
            pixelFill(9, 11, 100, fullblue);
        }
        if(soundLevel[3] >= 1000 && soundLevel[3] < 2000) {
            pixelFill(9, 11, 150, fullblue);
        }
        if(soundLevel[3] >= 2000 && soundLevel[3] < 3000) {
            pixelFill(9, 11, 200, fullblue);
        }
        if(soundLevel[3] >= 3000) {
            pixelFill(9, 11, 255, fullblue);
        }
        //Sound Level 4
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[4] = analogRead(OUT);
        Serial.printf("Level 4\n Value %i\n", soundLevel[4]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[4] < 400) {
            pixelFill(12, 13, 50, fullgreen);
        }
        if(soundLevel[4] >= 400 && soundLevel[4] < 1000) {
            pixelFill(12, 13, 100, fullgreen);
        }
        if(soundLevel[4] >= 1000 && soundLevel[4] < 2000) {
            pixelFill(12, 13, 150, fullgreen);
        }
        if(soundLevel[4] >= 2000 && soundLevel[4] < 3000) {
            pixelFill(12, 13, 200, fullgreen);
        }
        if(soundLevel[4] >= 3000) {
            pixelFill(12, 13, 255, fullgreen);
        }
        //Sound Level 5
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[5] = analogRead(OUT);
        Serial.printf("Level 5\n Value %i\n", soundLevel[5]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[4] < 400) {
            pixelFill(14, 15, 50, fullyellow);
        }
        if(soundLevel[5] >= 400 && soundLevel[5] < 1000) {
            pixelFill(14, 15, 100, fullyellow);
        }
        if(soundLevel[5] >= 1000 && soundLevel[5] < 2000) {
            pixelFill(14, 15, 150, fullyellow);
        }
        if(soundLevel[5] >= 2000 && soundLevel[5] < 3000) {
            pixelFill(14, 15, 200, fullyellow);
        }
        if(soundLevel[5] >= 3000) {
            pixelFill(14, 15, 255, fullyellow);
        }
        //Sound Level 6
        digitalWrite(STROBE, LOW);
        delayMicroseconds(72);
        soundLevel[6] = analogRead(OUT);
        Serial.printf("Level 6\n Value %i\n", soundLevel[6]);
        digitalWrite(STROBE, HIGH);
        delayMicroseconds(72); 
        if(soundLevel[6] < 400) {
            pixelFill(14, 15, 50, orange);
        }
        if(soundLevel[6] >= 400 && soundLevel[6] < 1000) {
            pixelFill(16, 16, 100, orange);
        }
        if(soundLevel[6] >= 1000 && soundLevel[6] < 2000) {
            pixelFill(16, 16, 150, orange);
        }
        if(soundLevel[6] >= 2000 && soundLevel[6] < 3000) {
            pixelFill(16, 16, 200, orange);
        }
        if(soundLevel[6] >= 3000) {
            pixelFill(16, 16, 255, orange);
        } 
  }
  if(distance < distanceThreshold) {
    distance = analogRead(MOTIONSENSOR);
    myDisplay.clearDisplay();
    myDisplay.display();
    pixel.clear();
    pixel.show();
  } 
}

//WEBHOOK 
void subscriptionHandler(const char *event, const char *data) {

    JSONValue outerObj = JSONValue::parseCopy(data);
    JSONObjectIterator iter(outerObj);
    while(iter.next()){
        Serial.printf("key=%s value=%s\n", (const char*) iter.name(), (const char*) iter.value().toString());
        myDisplay.clearDisplay();
        myDisplay.display();
        myDisplay.setRotation(2);
        myDisplay.setCursor (1,1);
        myDisplay.setTextSize(2);
        myDisplay.setTextColor(WHITE);
        myDisplay.printf("%s %s\n", (const char*) iter.name(), (const char*) iter.value().toString());
        myDisplay.display();
    }
}

//NEOPIXEL FUNCTIONS
int pixelFill(int startPixel, int endPixel, int brightness, int hexColor) {

  // pixel.clear();
  // pixel.show();
  pixel.setBrightness(brightness);
  for(int i = startPixel; i < endPixel; i++) {
    pixel.setPixelColor(i, hexColor);
  }
  pixel.show();
  delay(100);
  // pixel.clear();
  // pixel.show();

  return(endPixel);
}