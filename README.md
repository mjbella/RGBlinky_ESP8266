# RGBlinky_ESP8266

Description
===========
The goal of this Arduino project is to time sync an arbitrary number of nodes
with ws2812 RGB LEDs. We are using this breakout board
[HiLetgo-NodeMCU](http://amazon.com/HiLetgo-Version-NodeMCU-Internet-Development/dp/B010O1G1ES/ref=sr_1_3?s=electronics&ie=UTF8&qid=1500244397&sr=1-3&keywords=nodemcu)

Getting Started
===============
## Get the repo
git clone this repo to your machine and rename the parent folder to ''

## Get the serial port driver
Depending on the board you are using you might need to download and install
a driver. If you are using the HiLetgo-NodeMCU then you will need to download
and install the CP2102 driver from SiLabs
[http://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers](http://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

## Get the libraries
Download a zip folder of painless mesh from [painlessMesh](https://gitlab.com/BlackEdder/painlessMesh),
then go to the download logo in the upper right hand corner and select 'Download zip' under source code.

You will need to grab [ArduinoJson](https://github.com/bblanchon/ArduinoJson). 
Click on the button that says 'Clone or download' in the right upperhand
corner, and click Download ZIP.

You will also need to grab
[NeoPixelBus](https://github.com/Makuna/NeoPixelBus). 
Click on the button that says 'Clone or download' in the right upperhand
corner, and click Download ZIP.

## Install the libraries
Install all of these zipped libraries into you Arduino environment by going to
'Sketch' -> 'Include Library' -> 'Add .ZIP Library...' then select the zip
archive you downloaded.

## Setup the Arduino environment
Under 'Tools' in your Arduino you need to have the follow settings:
* Board: NodeMCU 0.9 (ESP-12 Module)
* CPU Frequency: "80MHz"
* Flash Size
* Upload Speed: 115200
* Port: /dev/

The upload speed and port will depend on the exact board you have and on your
operating system. You should be able to determine the upload speed from the
documentation for the board you have, and you should see a new device appear in
the port list after you have the board plugged in and you have the driver
installed.

