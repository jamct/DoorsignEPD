#define DEBUG 1
#define BASECAMP_NOMQTT
#include <Basecamp.hpp>

//Define your display type here: 2.9, 4.2 (bw and bwr) or 7.5 (bw or bwr) inches are supported:
#define DISPLAY_TYPE '4.2bwr'

#define FactorSeconds 1000000LL

Basecamp iot;
#include <GxEPD.h>

#if DISPLAY_TYPE == '1.5'
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>      // 1.54" b/w
bool hasRed = false;
String displayType = "1.5";
#endif
#if DISPLAY_TYPE == '2.9'
#include <GxGDEH029A1/GxGDEH029A1.cpp>      // 2.9" b/w
bool hasRed = false;
String displayType = "2.9";
#endif
#if DISPLAY_TYPE == '4.2'
#include <GxGDEW042T2/GxGDEW042T2.cpp>      // 4.2" b/w
bool hasRed = false;
String displayType = "4.2";
#endif
#if DISPLAY_TYPE == '4.2bwr'
#include <GxGDEW042Z15/GxGDEW042Z15.cpp>       // 4.2" b/w/r
bool hasRed = true;
String displayType = "4.2bwr";
#endif
#if DISPLAY_TYPE == '7.5'
#include <GxGDEW075T8/GxGDEW075T8.cpp>      // 7.5" b/w
bool hasRed = false;
String displayType = "7.5";
#endif
#if DISPLAY_TYPE == '7.5bwr'
#include <GxGDEW075Z09/GxGDEW075Z09.cpp>      // 7.5" b/w/r
bool hasRed = true;
String displayType = "7.5bwr";
#endif
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/FreeMonoBold9pt7b.h>

GxIO_Class io(SPI, SS, 17, 16);
GxEPD_Class display(io, 16, 4);
int value = 0;
bool connection = false;
bool production = false;

void setup() {
  iot.begin();
  display.init();
  const GFXfont* f = &FreeMonoBold9pt7b;
  display.setTextColor(GxEPD_BLACK);
  iot.web.addInterfaceElement("ImageHost", "input", "Server to load image from (host name or IP address):", "#configform", "ImageHost");
  iot.web.addInterfaceElement("ImageAddress", "input", "Address to load image from (path on server, starting with / e.g.: /index.php/?debug=false&[...] ):", "#configform", "ImageAddress");
  iot.web.addInterfaceElement("ImageWait", "input", "Sleep time (to next update) in seconds:", "#configform", "ImageWait");
  iot.web.addInterfaceElement("ProductionMode", "input", "Production mode  (if set to 'true', deep sleep will be activated, this config page will be down.)", "#configform", "ProductionMode");


  if (iot.configuration.get("ProductionMode") != "true" ) {

    if (iot.configuration.get("ImageWait").toInt() < 10) {
      iot.configuration.set("ImageWait", "60");
    }
    if (iot.configuration.get("ProductionMode").length() != 0 ) {
      iot.configuration.set("ProductionMode", "false");
    }

    if (iot.configuration.get("WifiConfigured") != "True") {

      display.fillScreen(GxEPD_WHITE);
      display.setRotation(1);
      display.setFont(f);
      display.setCursor(0, 0);
      display.println();
      display.println("Wifi not configured!");
      display.println("Connect to hotspot 'ESP32' and open 192.168.4.1");
      display.update();
    } else {

      int retry = 0;
      while ((WiFi.status() != WL_CONNECTED) && (retry < 20)) {
        retry++;
        delay(500);
      }
      if (retry == 20 )
      {
        connection = false;
        display.fillScreen(GxEPD_WHITE);
        display.setRotation(1);
        display.setFont(f);
        display.setCursor(0, 0);
        display.println();
        display.println("");
        display.println("Could not connect to " + iot.configuration.get("WifiEssid") );
        display.update();


      } else {
        connection = true;
        if (iot.configuration.get("ImageHost").length() < 1 || iot.configuration.get("ImageAddress").length() < 1 ) {
          display.fillScreen(GxEPD_WHITE);
          display.setRotation(1);
          display.setFont(f);
          display.setCursor(0, 0);
          display.println();
          display.println("");
          display.println("Image server not configured.");
          display.println("Open " + WiFi.localIP().toString() + " in your browser and set server address and path.");
          display.update();
          connection = false;
        }

      }



    }

  } else {
    production = true;
    int retry = 0;
    while ((WiFi.status() != WL_CONNECTED) && (retry < 20)) {
      Serial.println(".");
      retry++;
      delay(500);
    }
    if (retry < 20 ) {
      connection = true;
    }

  }

}

void loop() {

  if (connection == true) {
    String url =  iot.configuration.get("ImageAddress") + "&display=" + displayType;
    boolean currentLineIsBlank = true;
    WiFiClient client;
    delay(5000);
    ++value;

    const int httpPort = 80;
    const char* host = iot.configuration.get("ImageHost").c_str();

    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + iot.configuration.get("ImageHost") + "\r\n" +
                 "Connection: close\r\n\r\n");

    int x = 0;
    int y = 0;
    int start = 0;
    bool data = false;
    String header;

    display.eraseDisplay(true);

    while (client.available()) {
      char byte = client.read();

      if (data == false) {
        header +=  byte;
      }

      if (byte == '\n' && currentLineIsBlank && data == false) {
        data = true;

        if (header.indexOf("X-productionMode: false") > 0) {
          iot.configuration.set("ProductionMode", "false");
          production = false;
        }
        if (header.indexOf("X-productionMode: true") > 0) {
          iot.configuration.set("ProductionMode", "true");
          production = true;
        }
      }
      if (byte == '\n' && data == false) {
        currentLineIsBlank = true;
      } else if (byte != '\r' && data == false) {
        currentLineIsBlank = false;
      }

      if (data) {
        if (start < 1) {
          start++;
        } else {


          if (hasRed == true) {
            for (int b = 7; b >= 0; b -= 2) {
              int bit = bitRead(byte, b);
              int bit2 = bitRead(byte, b - 1);

              if ((bit == 1) && (bit2 == 1)) {
                display.drawPixel(x, y, GxEPD_BLACK);
              } else {
                if ((bit == 0) && (bit2 == 1)) {
                  display.drawPixel(x, y, GxEPD_RED);
                } else {
                  display.drawPixel(x, y, GxEPD_WHITE);
                }
              }
              x++;

              if  (x == GxEPD_WIDTH) {
                y++;
                x = 0;
              }
            }
          } else {

            for (int b = 7; b >= 0; b--) {
              int bit = bitRead(byte, b);

              if (bit == 1) {
                display.drawPixel(x, y, GxEPD_BLACK);
              } else {
                display.drawPixel(x, y, GxEPD_WHITE);
              }

              x++;

              if  (x == GxEPD_WIDTH) {
                y++;
                x = 0;
              }
            }
          }

        }

      }
    }
    display.update();
    Serial.println("Image loaded.");
  }
  if (production == true) {

    int SleepTime = iot.configuration.get("ImageWait").toInt();
    esp_sleep_enable_timer_wakeup(FactorSeconds * SleepTime);
    Serial.println("Going to sleep now...");
    esp_deep_sleep_start();
  } else {
    delay(5000);
    Serial.println("Setup: Not going to sleep. Use web config to setup.");
  }
};
