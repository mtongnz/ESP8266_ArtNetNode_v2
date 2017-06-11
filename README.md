# ESP8266_ArtNetNode_v2
ESP8266 based WiFi ArtNet V4 to DMX, RDM and LED Pixels

This is a complete rewrite of my previous project found here: http://www.instructables.com/id/ESP8266-Artnet-to-DMX/

An Instructable detailing how to setup pixel mapping using Jinx can be found here: https://www.instructables.com/id/Artnet-LED-Pixels-With-ESP8266/

If you find this helpful and you're feeling generous, I'd love for you to buy me a beer or some gear: https://www.paypal.me/mtongnz.  Thanks heaps to all those who have donated.  It's really appreciated

## Prizes for Fixing Stuff
There are 3 prizes up for grabs for people who fix/add some of the issues/features I haven't had time to finish yet.

Have a look here for details:  https://github.com/mtongnz/ESP8266_ArtNetNode_v2/blob/master/prizes.md

## Which file to flash
If you have a larger ESP with access to all the pins in the schematic, then download the standard bin file to ensure you get both outputs with RDM and also the settings reset function.

ESP01:  This disables the second output and puts the DIR_A onto pin GPIO2.

NO_RESET:  This disables the startup "reset of settings" funtionality.  It is not recommended and is only provided for compatibility with devices without GPIO14 available or for those of you with an older version of my hardware.

WEMOS:  This is compiled for Wemos D1 and similar boards.  It uses the 4M (3M SPIFFS) compiler option.

## Known Issues & Feedback
Please see the issues tab for known issues or to submit bugs or suggestions

## Schematic notes
esp_Artnet_RDM_isolated_v2.jpg is the latest version.  It uses APA106 LEDs for status.  This frees up more pins for future use - possibly with an LCD or more outputs.  You could modify the source if you wish to use normal LEDs instead.

The following notes are for the older schematic but some may apply to the current schematic so I've left it here:
 - Pin numbers used are ESP8266 GPIO numbers.  NodeMCU & Wemos boards use a different numbering system.  See below for details
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

## Flashing firmware
Please note that the device will generally need a power cycle after updating the firmware.

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

### Restore Factory Defaults
I have allowed for 2 methods to restore the factory default settings: using a dedicated factory reset button on GPIO14 or multiple power cycles.

Method 1: Hold GPIO14 to GND while the device boots.  This method isn't available for the ESP01 or NO_RESET builds.

Method 2: Allow the esp8266 about 1-4 seconds to start, then reset it (or power cycle).  Do this at least 5 times to restore factory default settings.

### DMX Workshop
I have implemented as many DMX Workshop/ArtNet V4 features as I possibly could.  You can change settings such as merge mode, IP address, DHCP, port addresses, node name...  Most of these are also available via the web UI.

## Features
 - sACN and ArtNet V4 support
 - 2 full universes of DMX output with full RDM support for both outputs
 - Up to 1360 ws2812(b) pixels - 8 full universes
 - DMX/RDM out one port, ws2812(b) out the other
 - DMX in - send to any ArtNet device
 - Web UI with mobile support
 - Web UI uses AJAX & JSON to minimize network traffic used & decrease latency
 - Full DMX Workshop support
 - Pixel FX - a 12 channel mode for ws2812 LED pixel control

## To be done
### Scene storage
I have not yet implemented the Scene Storage feature from my previous project.  I wish to improve on it and allow for making chases or effects.
### Handle artRdmSub packets
I haven't yet implemented the artRdmSub messages.  This doesn't affect usability as they are optional and all devices must support artRdm packets.  The artRdmSub messages do provide a smaller network load and this should be coming shortly.
### Pixels other than WS2812
A few people have asked for this and it shouldn't be too hard, except for the Pixel FX engine integration.

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
| 6 | Colour 1  | 0 - 255   | Red |
| 7 |           | 0 - 255   | Green |
| 8 |           | 0 - 255   | Blue |
| 9 | Colour 2  | 0 - 255   | Red |
| 10 |          | 0 - 255   | Green |
| 11 |          | 0 - 255   | Blue |
| 12 | Modify   | 0 - 255   | *Modify FX |

Modify FX is only currently used for the Static effect and is used to resize colour 1 within the overall size.

## NodeMCU & Wemos Pins
These boards use strange numbering that doesn't match the ESP8266 numbering.  Here are the main hookups needed:

| NodeMCU & Wemos | ESP8266 GPIO | Purpose |
|-----------------|--------------|---------|
| TX | GPIO1 | DMX_TX_A |
| D4 | GPIO2 | DMX_TX_B |
| RX | GPIO3 | DMX_RX (for A & B) |
| D1 | GPIO5 | DMX_DIR_A |
| D0 | GPIO16 | DMX_DIR_B |

## Special Thanks To
I'd like to thank these people.  They have either contributed with donations, a large amount of testing and feedback, or with their own code/projects that have been of assistance or inspiration.

 - Tristan Thiltges
 - Cyprien Leduc
 - Jochen Schefe
 - Jan Raeymaekers
 - Wiktor Kaluzny
 - Jean-Michel Blouin
 - Tobias Schulz
 - Ben Small
 - Harald Müller
 - Volodymyr Bereza
 - Felix Hartmann
 - Ruud Leemans
 - Bogumil palewicz
 - Anton Manchenko
 - Bogumil Palewicz
 - Marcel Dolnak
 - Paul Lim
 - Geo Karavasilis
 - [Claude Heintz](https://github.com/claudeheintz/)
 - [Shelby Merrick (Forkineye)](https://github.com/forkineye)
