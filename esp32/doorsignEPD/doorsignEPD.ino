#define DEBUG 1
#include <Basecamp.hpp>

//Define yout display type here: 2.9, 4.2 or 7.5 inches are supported:
#define DISPLAY_TYPE '2.9'
#define FactorSeconds 1000000

Basecamp iot;
#include <GxEPD.h>

#if DISPLAY_TYPE == '2.9'
#include <GxGDEH029A1/GxGDEH029A1.cpp>      // 2.9" b/w
#endif
#if DISPLAY_TYPE == '4.2'
#include <GxGDEW042T2/GxGDEW042T2.cpp>      // 4.2" b/w
#endif
#if DISPLAY_TYPE == '7.5'
#include <GxGDEW075T8/GxGDEW075T8.cpp>      // 7.5" b/w
#endif
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

GxIO_Class io(SPI, SS, 17, 16);
GxEPD_Class display(io, 16, 4);
int value = 0;

void setup() {
  iot.begin();
  display.init();
  display.fillScreen(GxEPD_WHITE);

  if(iot.configuration.get("ImageWait").toInt()<10){
    iot.configuration.set("ImageWait", "60");
  }

  iot.web.addInterfaceElement("ImageHost", "input", "Server to load image from:", "#configform", "ImageHost");
  iot.web.addInterfaceElement("ImageAddress", "input", "Address to load image from::", "#configform", "ImageAddress");
  iot.web.addInterfaceElement("ImageWait", "input", "Sleep time (to next update) in seconds:", "#configform", "ImageWait");
}

void loop() {

  //String url = "/epaper/?debug=false&display=2.9&content=door_sign";
  String url =  iot.configuration.get("ImageAddress");  

  if(url.length()<1 ){
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(0, 0);
    display.println("Display not configured");
    delay(20000);
  }
  else{
  
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
               "Host: " + iot.configuration.get("HttpHost") + "\r\n" +
               "Connection: close\r\n\r\n");

  display.eraseDisplay();
  display.fillScreen(GxEPD_WHITE);
  display.update();

  int x = 0;
  int y = 0;
  int start = 0;
  bool data = false;

  while (client.available()) {
    char byte = client.read();

    if (byte == '\n' && currentLineIsBlank && data == false) {
      data = true;
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

        for (int b = 7; b >= 0; b--) {
          int bit = bitRead(byte, b);
          //  Serial.println(byte);

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
  display.update();
  int SleepTime = iot.configuration.get("ImageWait").toInt(); 
  esp_sleep_enable_timer_wakeup(SleepTime * FactorSeconds);
    Serial.println("Going to sleep now...");
  esp_deep_sleep_start();
  
  }
};