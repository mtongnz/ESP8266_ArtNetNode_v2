# ESP8266_ArtNetNode_v2
ESP8266 based WiFi ArtNet V4 to DMX, RDM and LED Pixels

This is a complete rewrite of my previous project found here: http://www.instructables.com/id/ESP8266-Artnet-to-DMX/

As this code is still in beta, the source is not yet publicly available.  The device may also be unstable or unsuitable for use.

Please read this entire page as some features are not yet implemented.  If you use this, please give me feedback so I can improove it.

If you find this helpful and you're feeling generous, I'd love for you to buy me a beer: https://www.paypal.me/mtongnz

## Known Issues & Feedback
Please see the issues tab for known issues or to submit bugs or suggestions

## Schematic notes
 - I have excluded any voltage convertion as I've found the ESP8266 has 5V tolerant pins. See here: http://ba0sh1.com/blog/2016/08/03/is-esp8266-io-really-5v-tolerant/
 - The outputs are not isolated electrically or optically at this point.
 - C1 - 5 can be any caps and are used to ensure each component has a stable power supply.  I use 100nF.
 - You can use just one output if desired.  Simply don't connect the second MAX485 and associated components.
 - The MAX485 can be substituted for any compatible RS-485 transeiver.

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
 - 2 full universes of DMX output
 - Full RDM support for both outputs
 - Web UI with mobile support
 - Web UI uses AJAX & JSON to minimize network traffic used & decrease latency
 - Full DMX Workshop support

## To be done
### Scene storage
I have not yet implemented the Scene Storage feature from my previous project.  I wish to improve on it and allow for making chases or effects.
### LED Pixels
This feature is requested a lot and it should be coming shortly.
### Handle artRdmSub packets
I haven't yet implemented the artRdmSub messages.  This doesn't affect usability as they are optional and all devices must support artRdm packets.  The artRdmSub messages do provide a smaller network load and this should be coming shortly.
### DMX Input
This is another feature a few people want.  I don't have plans to do this in the near future but it will come eventually.  The hardware as designed will support 1 input (from either DMX A or B) while still allowing an output.
