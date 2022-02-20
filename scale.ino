#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 16;


const char *ssid         = "XXX";
const char *password     = "YYY";
unsigned long posledniCas = 0;
float offset = 501;
float spulka = 200;
long reading = 0;   //vaha prazdne spulky
float vaha = 0;
float tare = 0;

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`


// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

HX711 scale;

void setup() {
  
  WiFi.begin ( ssid, password );
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 10 );
  }

  display.init();
  //display.flipScreenVertically();
  display.setContrast(255);

  ArduinoOTA.begin();
  ArduinoOTA.onStart([]() {
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    display.clear();
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
  });

  ArduinoOTA.onEnd([]() {
  });
  display.clear();
  display.setFont(ArialMT_Plain_24);
 display.drawString(display.getWidth() / 2, display.getHeight() / 2, "hello");
 display.display();

 pinMode(14, INPUT_PULLUP);
}

void loop() {
  ArduinoOTA.handle();
  unsigned long cas = millis();
  if(cas - posledniCas > 50){
    posledniCas = cas;
    
    if (scale.is_ready()) {
    reading = scale.read()/100;
    vaha = (reading/ 4.48)-offset-spulka-tare;
    int delka =  vaha/(  3.141* pow( 0.875, 2 ) *1.24  );
    float procenta = ((float)delka/330.0)*100;
    display.clear();
    display.drawString(0, 0, String((int)vaha) + "g");
    display.drawString(0, display.getHeight() / 2 -5, String( delka  ) + "m");
    display.drawString(75, display.getHeight()/2-5, String((int)procenta) + "%");
    display.drawProgressBar(4, 55, 120, 8, abs(procenta) );
    display.display();
  } else {
    //Serial.println("HX711 not found.");
  }
  }
  if(!digitalRead(14)){
    tare = vaha;
    delay(500);
  }
  
}
