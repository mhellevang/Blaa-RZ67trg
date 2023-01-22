# RZ67 ESP32 Trigger

A ESP32 project for triggering the Mamiya RZ67 analog camera electronically from a bluetooth client.

There is also an android app for controlling the device, see [Android Blaa RZ67](https://github.com/mhellevang/Android-Blaa-RZ67).

# What is the Mamiya RZ67?

The [Mamiya RZ67](https://en.wikipedia.org/wiki/Mamiya_RZ67) is a great analog medium format film camera by Mamiya,
first made in 1982. It's the pinnacle of medium format studio cameras. 

![Photo of Mamiya RZ67](assets/Mamiya_RZ67_Professional_and_a_Fujifilm_color_120_format_roll_film_(60_mm_wide).jpg)

# How do I get my own device?

This project is still very much a work in progress. 

I'll publish my recommendations for making your own device once I've made a prototype I'm happy with. If you want to start working on making your own device now, you can use the schematics and code in this repository as a starting point, but do expect to make some changes to get it to work for you. Tinkering is half the fun!

## Hardware

* 1x ESP32 to receive the bluetooth commands and trigger the camera
* 1x IRF520 MOSFET (or similar) to control the shutter release
* 1x 4P 2.54mm pitch dupont connector for the shutter cable
* Wires
* Battery (optional) TBD

## Camera connection

The camera has four pins for the shutter release. The pins are labeled as follows (left to right):

* 1: 6v. We don't need it for this project.
* 2: Ground
* 3: S1 switch
* 4: S2 switch

To trigger the shutter release, we need to connect the S1 and S2 pins together.
We'll use a MOSFET to connect S1 and S2. The MOSFET will be controlled by the ESP32.

## Wiring

* ESP32 pin 32 to IRF520 gate (trigger signal)
* ESP32 pin GND to IRF520 source (ground)
* Dupont 4 port connector should be wired as follows:
    * Port 1: Not neaded. Leave it empty
    * Port 2: IRF520 gate
    * Port 3: IRF520 drain
    * Port 4: IRF520 source

