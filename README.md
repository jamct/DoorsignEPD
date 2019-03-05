# Currently under development
We are working on a new version on a modern base. There will be a modern web server (served as a docker image too) and optimized code for Platform.io (which has a better dependency manager). Stay tuned!

# Dependency: ArduinoJson

This project depends on the Arduino library `ArduinoJson` which currently
breaks the build if you use the latest version 6 (cf. [arduinojson.org/v5/doc/](https://arduinojson.org/v5/doc/)). To overcome this issue, downgrade (for example using the Arduino library manager) to the most recent version 5 (currently 5.13.4). Ensure that you do not upgrade to version 6 accidentally via the automatic update function.

# DoorsignEPD
Project to build digital doorsign based on ESP32, Waveshare E-Paper-Display (2.9, 4.2 or 7.5 inch). The display can load image from Webserver and uses deep-sleep mode of ESP32 to save energy.
Images are generated on Webserver running PHP. Examples in this repository generate a weather-station, a doorsign for an office and a doorsign for a conference room.

## Getting started (Client)
To start, you need the Arduino IDE with dependencies installed. Hardware setup is described here: [ct.de/yrzv](https://ct.de/yrzv).

The example wiring as used in the article is shown in the table below.


| Display | ESP32 | Type                                       | Comment                   |
|:-------:|:-----:|--------------------------------------------|---------------------------|
|   BUSY  |   4   | GPIO, ESP Input                            | Low active, display busy  |
|   RST   |   16  | GPIO, ESP Output                           | Low active, reset display |
|    DC   |   17  | GPIO, Data / Command Selection, ESP Output | High: data, low: command  |
|    CS   |   5   | GPIO, Chip Select, ESP Output              | Low active                |
|   CLK   |   18  | SPI, SCK pin (clock)                       | Defined by ESP            |
|   DIN   |   23  | SPI, MOSI pin                              | Defined by ESP            |
|   GND   |  GND  | Ground                                     |                           |
|   3V3   |  3V3  | Supply voltage, 3.3V                       | 8mA refresh, ~5uA standby |

The exact wiring depends on your hardware. Check for each pin (GPIO) if they
are not occupied by for example LEDs.

## Getting started (Server)
The folder 'server' contains examples for content and outputs it in Byte-stream-format for ESP32. Copy the folder on a webserver with PHP installed and GD active (PHP >7.0).
In the URL you tell the server what to show (and for which display size):

* <address of server>/?debug=true&display=7.5&content=weather_station&scale=28 (displays a weather-station for a 7.5 inch display)
* <address of server>/?debug=true&display=2.9&content=door_sign&scale=22 (displays a door-sign for a 2.9 inch display)
* <address of server>/?debug=true&display=4.2&content=conference_room&scale=18 (displays a sign for a conference room for a 4.2 inch display)
* <address of server>/?debug=true&display=4.2&content=door_sign_csv&room=A 111 (displays a sign for a room. Use data from a csv file or google spreadsheet. Works with different rooms.

With Get-Parameter 'scale' you adjust size of the text. Set 'debug' to true to get a png-image and false for byte-Stream for ESP32. Parameter size is automatically added by ESP (depending on your display).

## Example contents
* `conference_room` (agenda for a meeting room)
* `door_sign` (list of people working in a room)
* `door_sign_csv` (list of people working in a room, information taken from a csv file or google spreadsheet)
* `static_image` (showing a random image (scaled to display size) from server/contents/static_image. Just put your image here)
* `weather_station` (showing demo temperature an weather with icons)
* `ical_calendar` (showing demo Calendar with 7.5-Display and portrait orientation | https://github.com/sabre-io/vobject is requred)

## Dependencies

This project depends on the following libraries. Please use the most recent
stable version -- especially for `GxEPD` and `Basecamp`. GxEPD currently has to
be downloaded and installed manually. The other library can be installed using
the Arduino library manager.

* [GxEPD](https://github.com/ZinggJM/GxEPD)
* [Basecamp](https://github.com/merlinschumacher/Basecamp)
* [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
* [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)

### Indirect dependencies
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) version 5.x!

### Tested dependencies

This project was tested with the following Library versions:

- GxEPD v3.0.4 from github
- Basecamp v0.1.8 via Arduino library manager
- Adafruit_GFX v1.3.6 via Arduino library manager
- AsyncTCP from github (ac551716aa655672cd3e0f1f2c137a710bccdc42, v1.0.3)
- ESPAsyncWebServer from github (95dedf7a2df5a0d0ab01725baaacb4f982dedcb2,
  v1.2.0)
- ArduinoJson v5.13.4 via Arduino library manager
- ESP32 Arduino core by espressif: version 1.0.1

## More information
This repository is part of article "Ausdauernde Infotafel" from German computer magazine "c't". Link: [ct.de/yrzv](https://ct.de/yrzv)

## To do
+ add better examples with real data

## New functions
Support for red-black-white display!
