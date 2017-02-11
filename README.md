# ESP8266_ArtNetNode_v2
ESP8266 based WiFi ArtNet V4 to DMX, RDM and LED Pixels

This is a complete rewrite of my previous project found here: http://www.instructables.com/id/ESP8266-Artnet-to-DMX/

As this code is still in beta, the source is not yet publicly available.  The device may also be unstable or unsuitable for use.

Please read this entire page as some features are not yet implemented.  If you use this, please give me feedback so I can improove it.

If you find this helpful and you're feeling generous, I'd love for you to buy me a beer: https://www.paypal.me/mtongnz

## New in beta4
 - Settings reset button added.  Simply hold GPIO14 to GND on booting the device to reset all the settings to default.  Ensure GPIO14 is held to 3.3V via a resistor for normal operation.
 - Pixel FX.  I have added a 12 channel mode with various effects for the ws2812 mode.  This is mainly for people with controllers with limited channels available to them.  See below for details on the channel mapping.

## Which file to flash
If you have a larger ESP with access to all the pins in the schematic, then download the standard bin file to ensure you get both outputs with RDM and also the settings reset function.

ESP01:  This disables the second output and puts the DIR_A onto pin GPIO2.

NO_RESET:  This disables the startup "reset of settings" funtionality.  It is not recommended and is only provided for compatibility with devices without GPIO14 available or for those of you with an older version of my hardware.

## Known Issues & Feedback
Please see the issues tab for known issues or to submit bugs or suggestions

## Schematic notes
 - The ESP01 allows for one full DMX/RDM/Pixel port only.  Use pin GPIO2 instead of GPIO5 for the RDM direction pin.
 - GPIO14 is used to reset default settings.  Tie to 3.3V with a resistor for normal operation.  Hold to GND while the device is booting to wipe the settings and restore the defaults.  This feauture isn't available for ESP01 and a NO_RESET firmware is available also.
 - The node_dmx_and_pix schematic is recommended as it allows for DMX with RDM & also ws2812(b) strips by using the convert_max485_to_pix
 - The convert_max485_to_pix schematic is not a DMX driver for the strips!  It simply converts the logic back to the ws2812(b) logic.
 - The node_pix_only schematic is for those of you who don't want DMX.  Note that all the DMX options are still in the firmware and may cause the pixel strips to do wierd things if selected.
 - I have excluded any voltage convertion as I've found the ESP8266 has 5V tolerant pins. See here: http://ba0sh1.com/blog/2016/08/03/is-esp8266-io-really-5v-tolerant/
 - The outputs are not isolated electrically or optically at this point.
 - C1 - 5 can be any caps and are used to ensure each component has a stable power supply.  I use 100nF.
 - You can use just one output if desired.  Simply don't connect the second MAX485 and associated components.
 - The MAX485 can be substituted for any compatible RS-485 transeiver.
 - The LEDs are not used at present so you can leave them out.

## Flashing firmware
### Web UI OTA from my previous project
Flash https://github.com/mtongnz/ESP8266_ArtNetNode_DMX to your device (if not already on there) using the Arduino IDE and then use the Web UI to upload the .bin available in this git.
### Direct bin flash
Follow instructions here:  http://www.instructables.com/id/Intro-Esp-8266-firmware-update/
### Arduino IDE
You can also use the "WebUpdate" found in Examples->ESP8266WebServer.  Make sure you change the Flash Size in the Tools menu to match your ESP8266s flash.  You'll need to allow about 700K for the sketch and upload or OTA will fail - this means that with a 1M unit, select the 1M(256K SPIFFS).

Instructions to find out your flash size (if unknown) are on my Instructable linked at the top of this page.

## Getting Started
### First Boot
On your first boot, the device will start a hotspot called "espArtNetNode" with a password of "byMtongnz2017" (case sensitive).  Login to the hotspot and goto 2.0.0.1 in a browser.

Note that the hotspot is, by default, only for accessing the settings page.  You'll need to enable Stand Alone mode in the web UI if you want to send ArtNet to the device in hotspot mode.
### Web UI
In hotspot mode, goto 2.0.0.1 and in Wifi mode goto whatever the device IP might be - either static or assigned by DHCP.

In the Wifi tab, enter your SSID and password.  Click save (it should go green and say Settings Saved).  Now click reboot and the device should connect to your Wifi.

If the device can't connect to the wifi or get a DHCP assigned address within (Start Delay) seconds, it will start the hotspot and wait for 30 seconds for you to connect.  If a client doesn't connect to the hotspot in time, the device will restart and try again.
### DMX Workshop
I have implemented as many DMX Workshop/ArtNet V4 features as I possibly could.  You can change settings such as merge mode, IP address, DHCP, port addresses, node name...  Most of these are also available via the web UI.

## Features
 - 2 full universes of DMX output with full RDM support for both outputs
 - Up to 1360 ws2812(b) pixels - 8 full universes
 - DMX/RDM out one port, ws2812(b) out the other
 - Web UI with mobile support
 - Web UI uses AJAX & JSON to minimize network traffic used & decrease latency
 - Full DMX Workshop support
 - Pixel FX - a 12 channel mode for ws2812 LED pixel control

## To be done
### Scene storage
I have not yet implemented the Scene Storage feature from my previous project.  I wish to improve on it and allow for making chases or effects.
### Handle artRdmSub packets
I haven't yet implemented the artRdmSub messages.  This doesn't affect usability as they are optional and all devices must support artRdm packets.  The artRdmSub messages do provide a smaller network load and this should be coming shortly.
### DMX Input
This is another feature a few people want.  I don't have plans to do this in the near future but it will come eventually.  The hardware as designed will support 1 input (from either DMX A or B) while still allowing an output.

## Pixel FX
To enable this mode, select WS2812 in the port settings and enter the number of pixels you wish to control.  Select '12 Channel FX'. 'Start Channel' is the DMX address of the first channel below.

Note: You still need to set the Artnet net, subnet and universe correctly.

| DMX Channel | Function | Values (0-255) |  |
|----|----|----|----|
| 1 | Intensity | 0 - 255   |               |
| 2 | FX Select | 0 - 49    | Static        |
|   |           | 50 - 74   | Rainbow       |
|   |           | 75 - 99   | Theatre Chase |
|   |           | 100 - 124 | Twinkle       |
| 3 | Speed     | 0 - 19    | Stop - Index Reset |
|   |           | 20 - 122  | Slow - Fast CW |
|   |           | 123 - 130 | Stop |
|   |           | 131 - 234 | Fast - Slow CCW |
|   |           | 235 - 255 | Stop |
| 4 | Position  | 0 - 127 - 255 | Left - Centre - Right |
| 5 | Size      | 0 - 255   | Small - Big   |
| 6 | Red 1     | 0 - 255   |  |
| 7 | Green 1   | 0 - 255   |  |
| 8 | Blue 1    | 0 - 255   |  |
| 9 | Red 2     | 0 - 255   |  |
| 10 | Green 2  | 0 - 255   |  |
| 11 | Blue 2   | 0 - 255   |  |
| 12 | Modify   | 0 - 255   | *Modify FX |

Modify FX is only currently used for the Static effect and is used to resize colour 1 within the overall size.
