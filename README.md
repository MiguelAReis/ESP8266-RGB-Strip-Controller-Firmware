# ESP8266-RGB-Strip-Controller-Firmware
Arduino Firmware for the ESP8266 RGB Strip Controller

You can find the hardware documentantion here -> https://docs.microinventions.net/designs/esp8266-rgb-strip-controller

The RGB leds are controlled by POST requests. 

In version V1.0 You have the following modes:

0. OFF - Turns all leds OFF
1. Solid Colour - Puts the leds on a solid colour
2. Transition between 2 colours - Transitions between two colours with a programmable time.
3. Rainbow - Transitions between the rainbow colors with a programmable time and brightness.
4. Disco - Goes through the rainbow colours without transitioning with a programmable time and brightness.

The posts request have the same format, you can try to communicate with the esp8266 using curl, for example:

```curl --data "mode=3&arg0=0&arg1=0&arg2=0&arg3=0&arg4=0&arg5=0&arg6=300&arg7=100" 192.168.0.103/mode ```

```mode``` is the mode of the rgb lights, ```arg0```,```arg1```,```arg2``` are the first colour RGB values from 0 to 1024, ```arg3```,```arg4```,```arg5``` are the second colours RGB values, ```arg6``` is the speed between each rgb state in microSeconds and ```arg7``` is the brightness from 0 to 100%.

I made an app for android using MIT App Inventor 2, and included the Project source and the .apk file in this repository, that allows me to control the lights from an android phone.
