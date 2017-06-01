#include <GPNBadge.hpp>
#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include "url-encode.h"
#include "rboot.h"
#include "rboot-api.h"
#include "math.h"
#include <FS.h>

#include <stdio.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)

#define UP      797
#define DOWN    639
#define RIGHT   536
#define LEFT    1024
#define OFFSET  40

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Badge badge;

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
  SPIFFS.begin();
  connectBadge();
  
  bno.begin();
  delay(300);
  
  rboot_config rboot_config = rboot_get_config();
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

  float azdiff = azimuth_want - azimuth;
  float eldiff = elevation_want - elevation;

  tft.fillScreen(BLACK);
  
/*
  tft.fillRect(0, 0, 128, 100, BLACK);
  tft.setCursor(0,0);
  
  tft.print("AZ badge: ");
  tft.println(azimuth);
  tft.print("EL badge: ");
  tft.println(elevation);
  tft.println("");
  
  tft.print("AZ diff: ");
  tft.println(azdiff);
  tft.print("EL diff: ");
  tft.println(eldiff);
  tft.println("");
  
  tft.print("AZ sat: ");
  tft.println(azimuth_want);
  tft.print("EL sat: ");
  tft.println(elevation_want);
*/
  
  //// convert polar to cartesian
  // invert and scale elevation
  // convert azimuth deg to rad and rotate to correct angle
  // set offset to center of the circles
  float x_sensor = ( ((90-elevation) / 90 * 60 ) * cos((azimuth*PI/180)-HALF_PI) ) + 63;
  float y_sensor = ( ((90-elevation) / 90 * 60 ) * sin((azimuth*PI/180)-HALF_PI) ) + 63;
  float x_want = ( ((90-elevation_want) / 90 * 60 ) * cos((azimuth_want*PI/180)-HALF_PI) ) + 63;
  float y_want = ( ((90-elevation_want) / 90 * 60 ) * sin((azimuth_want*PI/180)-HALF_PI) ) + 63;
  
  tft.drawCircle(63,63,20,WHITE); // 60 deg
  tft.drawCircle(63,63,40,WHITE); // 30 deg
  tft.drawCircle(63,63,60,WHITE); // 0 deg
  tft.drawFastVLine(63,3,121,WHITE); // y axis
  tft.drawFastHLine(3,63,121,WHITE); // x axis
  tft.fillCircle(x_sensor,y_sensor,5,GREEN); // position of sensor
  tft.fillCircle(x_want,y_want,3,RED); // position from gpredict, overlays larger sensor circle

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
  int joystick = getJoystick();
  char* ssid = "badge-rotor";
  char* password = "";
  
  if(!joystick){
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(IPAddress (10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
  } else {
    File wifiConf = SPIFFS.open("/wifi.conf", "r");

    if (!wifiConf) {
      tft.println("File not found!");
      tft.println("Switching to SoftAP");
      tft.writeFramebuffer();
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      WiFi.softAPConfig(IPAddress (10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
      delay(1000);
    } else {
      String configString;
      while (wifiConf.available()) {
        configString += char(wifiConf.read());
      }
      wifiConf.close();
      UrlDecode confParse(configString.c_str());
      Serial.println(configString);
      configString = String();
      ssid = confParse.getKey("ssid");
      password = confParse.getKey("pw");
      
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      tft.setFont();
      tft.setTextSize(1);
      tft.setTextColor(WHITE);
      tft.setCursor(0, 2);
      tft.println("Connecting to:");
      tft.println(ssid);
      tft.writeFramebuffer();
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        tft.print(".");
        tft.writeFramebuffer();
      }
    }
  }
  
  tft.fillScreen(BLACK);
  tft.setCursor(0, 110);
  tft.print("IP:");
  if(!joystick){
   tft.println(WiFi.softAPIP());
   tft.print("SoftAP: ");
   tft.print(ssid);
  } else {
    tft.println(WiFi.localIP());
    tft.print("STA: ");
    tft.print(ssid);
  }
  tft.writeFramebuffer();
  delay(500);
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

int getJoystick() {
  uint16_t adc = analogRead(A0);
  //Serial.println(adc);
  if (adc < UP + OFFSET && adc > UP - OFFSET)             return 1;
  else if (adc < DOWN + OFFSET && adc > DOWN - OFFSET)    return 2;
  else if (adc < RIGHT + OFFSET && adc > RIGHT - OFFSET)  return 3;
  else if (adc < LEFT + OFFSET && adc > LEFT - OFFSET)    return 4;
  if (digitalRead(GPIO_BOOT) == 1) return 5;
  return 0;
}
