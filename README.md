# RGBlinky_ESP8266

Description
===========
The goal of this ardunio project is to time sync an arbitrary number of nodes
with ws2812 RGB LEDs. We are using this breakout board
[HiLetgo-NodeMCU](amazon.com/HiLetgo-Version-NodeMCU-Internet-Development/dp/B010O1G1ES/ref=sr_1_3?s=electronics&ie=UTF8&qid=1500244397&sr=1-3&keywords=nodemcu)

Getting Started
===============
## Get the repo
git clone this repo to your machine and rename the parent folder to ''

## Get the serial port driver
Depenging on the board you are using you might need to download and install
a dirver. If you are using the HiLetgo-NodeMCU then you will need to download
and install the CP2102 driver from SiLabs
[http://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers](http://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

## Get the libraries
Download a zip folder of painless mesh from [painlessMesh](https://gitlab.com/BlackEdder/painlessMesh)
Go to the download logo in the upper right hand corner and select 'Download zip' under source code.

You will need to grab [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
Click on the button that says 'Clone or download' in the right upperhand
corner, and click Download ZIP.

You will also need to grab [NeoPixelBus](https://github.com/Makuna/NeoPixelBus)
Click on the button that says 'Clone or download' in the right upperhand
corner, and click Download ZIP.

## Install the libraries
Install all of these ziped libraries into you Arduino environment by going to
'Sketch' -> 'Include Library' -> 'Add .ZIP Library...' then select the zip
archive you downloaded.

## Setup the Arduino environment
Under 'Tools' in your Arduino you need to have the follow settings:
* Board: NodeMCU 0.9 (ESP-12 Module)
* CPU Frequency: "80MHz"
* Flash Size
* Upload Speed: 115200
* Port: Pick this setting after you plug your board in an


(The upload speed  might need to be different depending on the board
  you have)

