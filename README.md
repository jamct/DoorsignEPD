# DoorsignEPD
Project to build digital doorsign based on ESP32, Waveshare E-Paper-Display (2.9, 4.2 or 7.5 inch). The display can load image from Webserver and uses deep-sleep mode of ESP32 to save energy.
Images are generated on Webserver running PHP. Examples in this repository generate a weather-station, a doorsign for an office and a doorsign for a conference room.

## Getting started (Client)
To start, you need the Arduino IDE with dependencies installed. Hardware setup is described here: [ct.de/yrzv](https://ct.de/yrzv).


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

## Dependencies

- [GxEPD](https://github.com/ZinggJM/GxEPD), [Basecamp](https://github.com/merlinschumacher/Basecamp), [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library), [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)

## More information
This repository is part of article "Ausdauernde Infotafel" from German computer magazine "c't". Link: [ct.de/yrzv](https://ct.de/yrzv)

## To do
+ add better examples with real data (like ical-calendar)

## New functions
Support for red-black-white display!
