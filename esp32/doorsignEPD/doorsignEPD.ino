#include <AsyncTCP.h>

#define DEBUG 1

#include <Basecamp.hpp>

// Define your display type here: 2.9, 4.2 (bw and bwr) or 7.5 (bw or bwr) inches are supported:
// Default: 4.2bwr
#define DISPLAY_TYPE '4.2bwr'

// Default 5
#define CHIP_SELECT 5

// Default -1: disabled
#define STATUS_PIN -1

#define FactorSeconds 1000000LL
#define BASECAMP_NOMQTT

// This is the upper limit for the sleep time set by the server to prevent accidentally letting the display sleep for several days
#define MAX_SLEEP_TIME (60*60*24)

// Encrypted setup WiFi network
//Basecamp iot{Basecamp::SetupModeWifiEncryption::secured};

// Unencrypted setup WiFi network (default)
Basecamp iot;
AsyncClient client;

volatile bool tcpClientConnected = false;             //* We are currently receiving data
volatile bool tcpClientConnectionInProgress = false;  //* We are establishing a connection
volatile bool requestDoneInPeriod = false;            //* We already received data in this period

bool connection = false;                              //* WiFi connection established
bool production = false;                              //* We are in production mode and will go to deep sleep
bool setupMode  = false;                              //* Setup mode: The web interface has to be accessible. Not going to deep sleep

String sleepIntervalHeader = "X-sleepInterval:";      //* Name of the header for the sleep interval
long   sleepIntervalSetbyHeader = 0;                  //* Changed if the sleep interval is set by the server via the header

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

GxIO_Class io(SPI, CHIP_SELECT, 17, 16);
GxEPD_Class display(io, 16, 4);

void setup() {
  iot.begin();
  display.init();

  if (STATUS_PIN >= 0){
    pinMode(STATUS_PIN, OUTPUT);
  }

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
      setupMode = true;
      display.fillScreen(GxEPD_WHITE);
      display.setRotation(1);
      display.setFont(f);
      display.setCursor(0, 0);
      display.println();
      display.println("Wifi not configured!");
      display.println("Connect to hotspot 'ESP32' with the secret '" + iot.configuration.get("APSecret") + "' and open 192.168.4.1");
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

  // Create client
  client.setRxTimeout(10);            // 5 sec timeout
  client.onConnect(&onConnectHandler);
  client.onData(&onDataHandler);
  client.onDisconnect(&onDisconnectHandler);
  client.onTimeout(&onTimeoutHandler);
  client.onError(&onErrorHandler);
}

/** Draw the pixels to the screen
 *  
 *  @param  char *data    A char array
 *  @param  size_t len    Length of the char array
 *  @param  boolean start True if the begin of a new screen
 * 
 */
void drawPixels(char *data, size_t len, boolean start){
  static int x;
  static int y;
  if (start){
    x = 0;
    y = 0;
    // Not required
    //display.eraseDisplay(true);
  }

  Serial.println(String("Printing ") + len + " Bytes to the screen");
  for (size_t i=0; i<len; i++){

    if (hasRed == true) {
      for (int b = 7; b >= 0; b -= 2) {
        int bit = bitRead(data[i], b);
        int bit2 = bitRead(data[i], b - 1);

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
    } else {  // hasRead
      for (int b = 7; b >= 0; b--) {
        int bit = bitRead(data[i], b);
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

/**
 *  Handler called after connection is established
 */
void onConnectHandler(void *r, AsyncClient *client){
  Serial.println("OnConnect\n");
  tcpClientConnected = true;
  tcpClientConnectionInProgress = false;
  if (STATUS_PIN >= 0){
    digitalWrite(5, HIGH);
  }
  String url =  iot.configuration.get("ImageAddress") + "&display=" + displayType;
  String query = String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + iot.configuration.get("ImageHost") + "\r\n" +
                 "Connection: close\r\n\r\n";

  client->write(query.c_str());
}

/**
 * Handler called for each received data packet
 */
void onDataHandler(void *r, AsyncClient *client, void *data, size_t len){ 
  Serial.println(String("OnData: ") + len + " Bytes");
  int16_t endHeader = findEndHeader((char*) data, len);

  if (endHeader > 0){
    // We got the header
    char subbuf[endHeader + 1];
    memcpy(subbuf, data, endHeader);
    subbuf[endHeader] = 0;
    String header = String(subbuf);
    Serial.println(header);
    
    // handle header data
    if (header.indexOf("X-productionMode: false") > 0) {
      iot.configuration.set("ProductionMode", "false");
      production = false;
    }
    if (header.indexOf("X-productionMode: true") > 0) {
      iot.configuration.set("ProductionMode", "true");
      production = true;
    }
    Serial.println(String("ProductionMode: ") + production);

    // Let the user set the sleep interval via a HTTP-header
    if (header.indexOf(sleepIntervalHeader) > 0){
      int sleepStartIndex = header.indexOf(sleepIntervalHeader) + sleepIntervalHeader.length();
      int sleepStopIndex = header.indexOf("\r\n", sleepStartIndex);
      if (sleepStopIndex < 0){
        // End of header, use length as delimiter
        sleepStopIndex = endHeader;
      }

      // toInt() sets sleepIntervalSetbyHeader to zero in case of errors during the conversion
      sleepIntervalSetbyHeader = header.substring(sleepStartIndex, sleepStopIndex).toInt();
      if (sleepIntervalSetbyHeader > MAX_SLEEP_TIME){
        Serial.println("Too long sleep time. Limiting...");
        sleepIntervalSetbyHeader = MAX_SLEEP_TIME;
      }
      
    }
    
    // Handle remaining data bytes. 4 bytes for \r\n\r\n separation
    drawPixels((char*)data+endHeader+4, len-endHeader-4, true);
  } else {
    // No header -> directly draw to display
    drawPixels((char*)data, len, false);
  }
}

/**
 * Handler called in case of a timeout
 */
void onTimeoutHandler(void *r, AsyncClient *client, uint32_t timeout){
  Serial.println(String("Timeout ") + timeout);
  transmitDone();
}

/**
 * Handler called after a disconnect event
 */
void onDisconnectHandler(void *r, AsyncClient *client){
  Serial.println("OnDisconnect");
  display.update();
  transmitDone();
}

/**
 * Handler for the error cases
 */
void onErrorHandler(void *r, AsyncClient *client, int8_t error){
  Serial.println(String("Error:") + error);
  transmitDone();
}

/** Find the end to the HTTP header marked by "\r\n\r\n"
 *  
 *  @param char *buf    A char array
 *  @param  size_t len  Length of the char array
 *  @return The position of \r\n\r\n
 */
int16_t findEndHeader(char *buf, size_t len){
  const char *endString = "\r\n\r\n";
  for (int16_t i=0; i<len-4; i++){
    if (
      buf[i] == endString[0] &&
      buf[i+1] == endString[1] &&
      buf[i+2] == endString[2] &&
      buf[i+3] == endString[3]
      ) {
        return i;
      }
  }
  return -1;
}

/**
 * Called after the transmission is finished (error, connection closed, timeout)
 */
void transmitDone(){
  Serial.println("transmitDone");
  if (STATUS_PIN >= 0){
    digitalWrite(5, LOW);
  }
  tcpClientConnected = false;
  tcpClientConnectionInProgress = false;
  requestDoneInPeriod = true;
}


/**
 * The main loop
 */
void loop() {

  if (
    WiFi.status() == WL_CONNECTED &&
    !tcpClientConnected &&
    !tcpClientConnectionInProgress &&
    !requestDoneInPeriod
    ) {
      const int httpPort = 80;
      const char* host = iot.configuration.get("ImageHost").c_str();
      
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      } else {
        Serial.println("Wait till the client is connected");
      }    
    }

  requestDoneInPeriod = false;
  if (
    !production ||                          // Not in production mode
    tcpClientConnected ||                   // We are connected to the server
    setupMode                               // We are in setup mode
    ) {
      delay(10000);
      Serial.println("Not going to deep sleep. Reason:");
      if (!production) Serial.println("Not in production mode");
      if (tcpClientConnected) Serial.println("Ongoing connection");
      if (setupMode) Serial.println("In setup mode");
    } else {
      if (sleepIntervalSetbyHeader > 0){
        Serial.println(String("Using sleep interval set by header \"") + sleepIntervalHeader + "\":" + sleepIntervalSetbyHeader);
        esp_sleep_enable_timer_wakeup(FactorSeconds * (uint64_t)sleepIntervalSetbyHeader);
      } else {
        // No sleep time set via header or invalid value
        int SleepTime = iot.configuration.get("ImageWait").toInt();
        esp_sleep_enable_timer_wakeup(FactorSeconds * (uint64_t)SleepTime);
      }
      Serial.println("Going to deep sleep now...");
      esp_deep_sleep_start();
    }
};
