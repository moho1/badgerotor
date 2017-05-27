#include <GPNBadge.hpp>
#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>

#include "rboot.h"
#include "rboot-api.h"
#include <FS.h>

#include <stdio.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)

Badge badge;

const char* ssid = "rotor-moho";
const char* password = "moho-rotor";
WiFiServer server(4533);  //set up server
WiFiClient client;

Adafruit_BNO055 bno = Adafruit_BNO055(BNO055_ID, BNO055_ADDRESS_B);
imu::Vector<3> euler;

float azimuth, elevation;
float azimuth_want, elevation_want;

void setup() {
  // put your setup code here, to run once:
  badge.init();
  badge.setBacklight(true);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setFont();
  tft.fillScreen(BLACK);
  connectBadge();
  
  bno.begin();
  delay(300);
  
  rboot_config rboot_config = rboot_get_config();
  SPIFFS.begin();
  File f = SPIFFS.open("/rom"+String(rboot_config.current_rom),"w");
  f.println("Rotor\n");
  f.close();

  
}

void loop() {
  // put your main code here, to run repeatedly:
  euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  azimuth = euler.x();
  if ( azimuth < 180 ) {
    azimuth += 180;
  } else {
    azimuth -= 180;
  }
  elevation = euler.z();

  tft.fillRect(0, 0, 128, 50, BLACK);
  tft.setCursor(2,2);
  tft.print("Azimuth: ");
  tft.print(azimuth);
  tft.setCursor(2,12);
  tft.print("Elevation: ");
  tft.print(elevation);

  float azdiff = azimuth_want - azimuth;
  float eldiff = elevation_want - elevation;
  tft.setCursor(2,22);
  tft.print("AZdiff: ");
  tft.print(azdiff);
  tft.setCursor(2,32);
  tft.print("ELdiff: ");
  tft.print(eldiff);
  tft.writeFramebuffer();

  int azdiffdir = 1;
  int eldiffdir = 1;

  if (azdiff < 0) {
    azdiff *= -1;
    azdiffdir *= -1;
  }

  if (azdiff > 180) {
    azdiffdir *= -1;
    azdiff -= 360;
    azdiff *= -1;
  }
  
  if (eldiff < 0) {
    eldiff *= -1;
    eldiffdir *= -1;
  }

  if (eldiff > 180) {
    eldiffdir *= -1;
    eldiff -= 360;
    eldiff *= -1;
  }
  
  int colors[3];
  // 0:up, 1: left, 2: right, 3: down
  if (azdiffdir > 0) {
    HSVtoRGB(20, 255, azdiff*2, colors);
    pixels.setPixelColor(2, pixels.Color(colors[0], colors[1], colors[2]));
    pixels.setPixelColor(1, pixels.Color(0,0,0));
  } else {
    HSVtoRGB(20, 255, azdiff*2, colors);
    pixels.setPixelColor(1, pixels.Color(colors[0], colors[1], colors[2]));
    pixels.setPixelColor(2, pixels.Color(0,0,0));
  }

  if (eldiffdir > 0) {
    HSVtoRGB(20, 255, eldiff*2, colors);
    pixels.setPixelColor(0, pixels.Color(colors[0], colors[1], colors[2]));
    pixels.setPixelColor(3, pixels.Color(0,0,0));
  } else {
    HSVtoRGB(20, 255, eldiff*2, colors);
    pixels.setPixelColor(3, pixels.Color(colors[0], colors[1], colors[2]));
    pixels.setPixelColor(0, pixels.Color(0,0,0));
  }

  pixels.show();

  if (!client || !client.connected()) {
    if (client) {
      client.stop();
    }
    client = server.available();
    if (!client) {
      return;
    }
  }

  client.setTimeout(10);
  String req = client.readStringUntil('\n');
  char reqc[20];
  req.toCharArray(reqc, 20);
  if (sscanf(reqc, "P %f %f", &azimuth_want, &elevation_want) == 2) {
    client.printf("RPRT 0");
  } else if (req[0] == 'p') {
    client.printf("%3.6f\n%3.6f\n", azimuth, elevation);
  }
}

void connectBadge() {
  WiFi.softAP(ssid, password);

  delay(500);

  tft.setCursor(2, 100);
  tft.print("IP Address:");
  tft.setCursor(2, 110);
  tft.print(WiFi.softAPIP());
  tft.writeFramebuffer();

  server.begin();
}

void HSVtoRGB(int hue, int sat, int val, int colors[3]) {
  // hue: 0-359, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;
  if (sat == 0)
  {                     // Achromatic color (gray).
    colors[0] = val;
    colors[1] = val;
    colors[2] = val;
  }
  else 
  {
    base = ((255 - sat) * val) >> 8;
    switch(hue / 60)
    {
      case 0:
        r = val;
        g = (((val - base) * hue) / 60) + base;
        b = base;
        break;
      case 1:
        r = (((val - base) * (60 - (hue % 60))) / 60) + base;
        g = val;
        b = base;
        break;
      case 2:
        r = base;
        g = val;
        b = (((val - base) * (hue % 60)) / 60) + base;
        break;
      case 3:
        r = base;
        g = (((val - base) * (60 - (hue % 60))) / 60) + base;
        b = val;
        break;
      case 4:
        r = (((val - base) * (hue % 60)) / 60) + base;
        g = base;
        b = val;
        break;
      case 5:
        r = val;
        g = base;
        b = (((val - base) * (60 - (hue % 60))) / 60) + base;
        break;
    }
    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
  }
}
