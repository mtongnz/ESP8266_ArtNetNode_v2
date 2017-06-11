/*
ESP8266_ArtNetNode v2.0.0
Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/ESP8266_ArtNetNode_v2

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/

void ajaxHandle() {
  JsonObject& json = jsonBuffer.parseObject(webServer.arg("plain"));
  JsonObject& jsonReply = jsonBuffer.createObject();
  
  String reply;
  
  // Handle request to reboot into update mode
  if (json.containsKey("success") && json["success"] == 1 && json.containsKey("doUpdate")) {
    artRDM.end();
    
    jsonReply["success"] = 1;
    jsonReply["doUpdate"] = 1;
    
    jsonReply.printTo(reply);
    webServer.send(200, "application/json", reply);

    if (json["doUpdate"] == 1) {
      // Turn pixel strips off if they're on
      pixDriver.updateStrip(0, 0, deviceSettings.portApixConfig);
      pixDriver.updateStrip(1, 0, deviceSettings.portBpixConfig);
      
      deviceSettings.doFirmwareUpdate = true;
      eepromSave();
      
      doReboot = true;
    }
    
  // Handle load and save of data
  } else if (json.containsKey("success") && json["success"] == 1 && json.containsKey("page")) {
    if (ajaxSave((uint8_t)json["page"], json)) {
      ajaxLoad((uint8_t)json["page"], jsonReply);

      if (json.size() > 2)
        jsonReply["message"] = "Settings Saved";

    } else {
      jsonReply["success"] = 0;
      jsonReply["message"] = "Failed to save data.  Reload page and try again.";
    }
    
  // Handle reboots
  } else if (json.containsKey("success") && json.containsKey("reboot") && json["reboot"] == 1) {
    jsonReply["success"] = 1;
    jsonReply["message"] = "Device Restarting.";
    
    // Turn pixel strips off if they're on
    pixDriver.updateStrip(0, 0, deviceSettings.portApixConfig);
    pixDriver.updateStrip(1, 0, deviceSettings.portBpixConfig);
    
    doReboot = true;

  // Handle errors
  } 

  jsonReply.printTo(reply);
  webServer.send(200, "application/json", reply);
}

bool ajaxSave(uint8_t page, JsonObject& json) {
  // This is a load request, not a save
  if (json.size() == 2)
    return true;
  
  switch (page) {
    case 1:     // Device Status
      // We don't need to save anything for this.  Go straight to load
      return true;
      break;

    case 2:     // Wifi
      json.get<String>("wifiSSID").toCharArray(deviceSettings.wifiSSID, 40);
      json.get<String>("wifiPass").toCharArray(deviceSettings.wifiPass, 40);
      json.get<String>("hotspotSSID").toCharArray(deviceSettings.hotspotSSID, 20);
      json.get<String>("hotspotPass").toCharArray(deviceSettings.hotspotPass, 20);
      deviceSettings.hotspotDelay = (uint8_t)json["hotspotDelay"];
      deviceSettings.standAloneEnable = (bool)json["standAloneEnable"];

      eepromSave();
      return true;
      break;

    case 3:     // IP Address & Node Name
      deviceSettings.ip = IPAddress(json["ipAddress"][0],json["ipAddress"][1],json["ipAddress"][2],json["ipAddress"][3]);
      deviceSettings.subnet = IPAddress(json["subAddress"][0],json["subAddress"][1],json["subAddress"][2],json["subAddress"][3]);
      deviceSettings.gateway = IPAddress(json["gwAddress"][0],json["gwAddress"][1],json["gwAddress"][2],json["gwAddress"][3]);
      deviceSettings.broadcast = deviceSettings.ip | (~deviceSettings.subnet);
      //deviceSettings.broadcast = {~deviceSettings.subnet[0] | (deviceSettings.ip[0] & deviceSettings.subnet[0]), ~deviceSettings.subnet[1] | (deviceSettings.ip[1] & deviceSettings.subnet[1]), ~deviceSettings.subnet[2] | (deviceSettings.ip[2] & deviceSettings.subnet[2]), ~deviceSettings.subnet[3] | (deviceSettings.ip[3] & deviceSettings.subnet[3])};

      json.get<String>("nodeName").toCharArray(deviceSettings.nodeName, 18);
      json.get<String>("longName").toCharArray(deviceSettings.longName, 64);

      if (!isHotspot && (bool)json["dhcpEnable"] != deviceSettings.dhcpEnable) {
        
        if ((bool)json["dhcpEnable"]) {
          /*
          // Re-enable DHCP
          WiFi.begin(deviceSettings.wifiSSID, deviceSettings.wifiPass);

          // Wait for an IP
          while (WiFi.status() != WL_CONNECTED)
            yield();
          
          // Save settings to struct
          deviceSettings.ip = WiFi.localIP();
          deviceSettings.subnet = WiFi.subnetMask();
          deviceSettings.broadcast = {~deviceSettings.subnet[0] | (deviceSettings.ip[0] & deviceSettings.subnet[0]), ~deviceSettings.subnet[1] | (deviceSettings.ip[1] & deviceSettings.subnet[1]), ~deviceSettings.subnet[2] | (deviceSettings.ip[2] & deviceSettings.subnet[2]), ~deviceSettings.subnet[3] | (deviceSettings.ip[3] & deviceSettings.subnet[3])};
          */
          
          deviceSettings.gateway = INADDR_NONE;
          
        }
        /*
        else {
          // Set static IP
          WiFi.config(deviceSettings.ip,deviceSettings.ip,deviceSettings.ip,deviceSettings.subnet);
        }

        // Add any changes to artnet settings - this will send correct artpollreply
        artRDM.setIP(deviceSettings.ip, deviceSettings.subnet);
        artRDM.setDHCP(deviceSettings.dhcpEnable);
        */

        doReboot = true;
      }

      if (!isHotspot) {
        artRDM.setShortName(deviceSettings.nodeName);
        artRDM.setLongName(deviceSettings.longName);
      }
      
      deviceSettings.dhcpEnable = (bool)json["dhcpEnable"];
      
      eepromSave();
      return true;
      break;

    case 4:     // Port A
      {
        deviceSettings.portAprot = (uint8_t)json["portAprot"];
        bool e131 = (deviceSettings.portAprot == PROT_ARTNET_SACN) ? true : false;
        
        deviceSettings.portAmerge = (uint8_t)json["portAmerge"];
  
        if ((uint8_t)json["portAnet"] < 128)
          deviceSettings.portAnet = (uint8_t)json["portAnet"];
  
        if ((uint8_t)json["portAsub"] < 16)
        deviceSettings.portAsub = (uint8_t)json["portAsub"];
  
        for (uint8_t x = 0; x < 4; x++) {
          if ((uint8_t)json["portAuni"][x] < 16)
            deviceSettings.portAuni[x] = (uint8_t)json["portAuni"][x];
          
          if ((uint16_t)json["portAsACNuni"][x] > 0 && (uint16_t)json["portAsACNuni"][x] < 64000)
            deviceSettings.portAsACNuni[x] = (uint16_t)json["portAsACNuni"][x];

          artRDM.setE131(portA[0], portA[x+1], e131);
          artRDM.setE131Uni(portA[0], portA[x+1], deviceSettings.portAsACNuni[x]);
        }
  
        uint8_t newMode = json["portAmode"];
        uint8_t oldMode = deviceSettings.portAmode;
        bool updatePorts = false;

        #ifndef ONE_PORT
          // RDM and DMX input can't run together
          if (newMode == TYPE_DMX_IN && deviceSettings.portBmode == TYPE_RDM_OUT) {
            deviceSettings.portBmode = TYPE_DMX_OUT;
            dmxB.rdmDisable();
          }
        #endif
        
        if (newMode == TYPE_DMX_IN && json.containsKey("dmxInBroadcast"))
          deviceSettings.dmxInBroadcast = IPAddress(json["dmxInBroadcast"][0],json["dmxInBroadcast"][1],json["dmxInBroadcast"][2],json["dmxInBroadcast"][3]);
         
        
        if (newMode != oldMode) {
  
          // Store the nem mode to settings
          deviceSettings.portAmode = newMode;

          doReboot = true;
          /*
          if (oldMode == TYPE_WS2812) {
            doReboot = true;
            
            // Set pixel strip length to zero
            pixDriver.updateStrip(0, 0, deviceSettings.portApixConfig);
  
            // Close ports from pixels - library handles if they dont exist
            for (uint8_t x = 2; x <= 4; x++)
              artRDM.closePort(portA[0], portA[x]);

            // Start our DMX port
            dmxA.begin(DMX_DIR_A, artRDM.getDMX(portA[0], portA[1]));
            
          } else if (oldMode == TYPE_DMX_IN)
            dmxA.dmxIn(false);
          else if (oldMode == TYPE_RDM_OUT)
            dmxA.rdmDisable();

          // Start DMX output with no DMX
          if (newMode == TYPE_DMX_OUT) {
            
            
            artRDM.setPortType(portA[0], portA[1], DMX_OUT);

          // Start DMX output with RDM
          } else if (newMode == TYPE_RDM_OUT) {
            dmxA.rdmEnable(ESTA_MAN, ESTA_DEV);
            dmxA.rdmSetCallBack(rdmReceivedA);
            dmxA.todSetCallBack(sendTodA);
            artRDM.setPortType(portA[0], portA[1], RDM_OUT);

          // Start DMX input
          } else if (newMode == TYPE_DMX_IN) {
            dmxA.dmxIn(true);
            dmxA.setInputCallback(dmxIn);
            
            artRDM.setPortType(portA[0], portA[1], DMX_IN);

          // Start WS2812 output
          } else if (newMode == TYPE_WS2812) {
            doReboot = true;

            dmxA.end();
            
            artRDM.setPortType(portA[0], portA[1], TYPE_DMX_OUT);
            updatePorts = true;
            
            // Initialize the pixel strip
            pixDriver.setStrip(0, DMX_TX_A, deviceSettings.portAnumPix, deviceSettings.portApixConfig);
            
          }
          */
        }
        
        // Update the Artnet class
        artRDM.setNet(portA[0], deviceSettings.portAnet);
        artRDM.setSubNet(portA[0], deviceSettings.portAsub);
        artRDM.setUni(portA[0], portA[1], deviceSettings.portAuni[0]);
        artRDM.setMerge(portA[0], portA[1], deviceSettings.portAmerge);

        // Lengthen or shorten our pixel strip & handle required Artnet ports
        if (newMode == TYPE_WS2812 && !doReboot) {
          // Get the new & old lengths of pixel strip
          uint16_t newLen = (json.containsKey("portAnumPix")) ? (uint16_t)json["portAnumPix"] : deviceSettings.portAnumPix;
          if (newLen > 680)
            newLen = 680;
          
          uint16_t oldLen = deviceSettings.portAnumPix;
          bool lenChanged = false;
  
          // If pixel size has changed
          if (newLen <= 680 && oldLen != newLen) {
            // Update our pixel strip
            deviceSettings.portAnumPix = newLen;
            pixDriver.updateStrip(1, deviceSettings.portAnumPix, deviceSettings.portApixConfig);
  
            lenChanged = true;

            // If the old mode was pixel map then update the Artnet ports
            if (deviceSettings.portApixMode == FX_MODE_PIXEL_MAP)
              updatePorts = true;
          }
            
          // If the old mode was 12 channel FX, update oldLen to represent the number of channels we used
          if (deviceSettings.portApixMode == FX_MODE_12)
            oldLen = 12;
  
          // If our mode changes then update the Artnet ports
          if (deviceSettings.portApixMode != (uint8_t)json["portApixMode"])
            updatePorts = true;
  
          // Store the new pixel mode
          deviceSettings.portApixMode = (uint8_t)json["portApixMode"];
  
          // If our new mode is FX12 then we need 12 channels & store the start address
          if (deviceSettings.portApixMode == FX_MODE_12) {
            if ((uint16_t)json["portApixFXstart"] <= 501 && (uint16_t)json["portApixFXstart"] > 0)
              deviceSettings.portApixFXstart = (uint16_t)json["portApixFXstart"];
            newLen = 12;
          }

          // If needed, open and close Artnet ports
          if (updatePorts) {
            for (uint8_t x = 1, y = 2; x < 4; x++, y++) {
              uint16_t c = (x * 170);
              if (newLen > c)
                portA[y] = artRDM.addPort(portA[0], x, deviceSettings.portAuni[x], TYPE_DMX_OUT, deviceSettings.portAmerge);
              else if (oldLen > c)
                artRDM.closePort(portA[0], portA[y]);
            }
          }
  
          // Set universe and merge settings (port 1 is done above for all port types)
          for (uint8_t x = 1, y = 2; x < 4; x++, y++) {
            if (newLen > (x * 170)) {
              artRDM.setUni(portA[0], portA[y], deviceSettings.portAuni[x]);
              artRDM.setMerge(portA[0], portA[y], deviceSettings.portAmerge);
            }
          }
        }
  
        artRDM.artPollReply();
  
        eepromSave();
        return true;
      }
      break;

    case 5:     // Port B
      #ifndef ONE_PORT
      {
        deviceSettings.portBprot = (uint8_t)json["portBprot"];
        bool e131 = (deviceSettings.portBprot == PROT_ARTNET_SACN) ? true : false;
        
        deviceSettings.portBmerge = (uint8_t)json["portBmerge"];
  
        if ((uint8_t)json["portBnet"] < 128)
          deviceSettings.portBnet = (uint8_t)json["portBnet"];
  
        if ((uint8_t)json["portBsub"] < 16)
        deviceSettings.portBsub = (uint8_t)json["portBsub"];
  
        for (uint8_t x = 0; x < 4; x++) {
          if ((uint8_t)json["portBuni"][x] < 16)
            deviceSettings.portBuni[x] = (uint8_t)json["portBuni"][x];
          
          if ((uint16_t)json["portBsACNuni"][x] > 0 && (uint16_t)json["portBsACNuni"][x] < 64000)
            deviceSettings.portBsACNuni[x] = (uint16_t)json["portBsACNuni"][x];

          artRDM.setE131(portB[0], portB[x+1], e131);
          artRDM.setE131Uni(portB[0], portB[x+1], deviceSettings.portBsACNuni[x]);
        }
  
        uint8_t newMode = json["portBmode"];
        uint8_t oldMode = deviceSettings.portBmode;
        bool updatePorts = false;
        
        // RDM and DMX input can't run together
        if (newMode == TYPE_RDM_OUT && deviceSettings.portAmode == TYPE_DMX_IN)
          newMode = TYPE_DMX_OUT;
        
        if (newMode != oldMode) {
          
          // Store the nem mode to settings
          deviceSettings.portBmode = newMode;

          doReboot = true;

          /*
          if (oldMode == TYPE_WS2812) {
            doReboot = true;
            
            // Set pixel strip length to zero
            pixDriver.updateStrip(1, 0, deviceSettings.portBpixConfig);
  
            // Close ports from pixels - library handles if they dont exist
            for (uint8_t x = 2; x <= 4; x++)
              artRDM.closePort(portB[0], portB[x]);

            // Start our DMX port
            dmxB.begin(DMX_DIR_B, artRDM.getDMX(portB[0], portB[1]));
            
            
          } else if (oldMode == TYPE_RDM_OUT)
            dmxB.rdmDisable();
          
          

          // Start DMX output with no DMX
          if (newMode == TYPE_DMX_OUT) {
            artRDM.setPortType(portB[0], portB[1], DMX_OUT);

          // Start DMX output with RDM
          } else if (newMode == TYPE_RDM_OUT) {
            dmxB.rdmEnable(ESTA_MAN, ESTA_DEV);
            dmxB.rdmSetCallBack(rdmReceivedB);
            dmxB.todSetCallBack(sendTodB);
            artRDM.setPortType(portB[0], portB[1], RDM_OUT);

          // Start WS2812 output
          } else if (newMode == TYPE_WS2812) {
            doReboot = true;

            
            //dmxB.end();
            artRDM.setPortType(portB[0], portB[1], TYPE_DMX_OUT);
            updatePorts = true;
            
            // Initialize the pixel strip
            pixDriver.setStrip(1, DMX_TX_B, deviceSettings.portBnumPix, deviceSettings.portBpixConfig);
            
          }
          */
        }
        
        // Update the Artnet class
        artRDM.setNet(portB[0], deviceSettings.portBnet);
        artRDM.setSubNet(portB[0], deviceSettings.portBsub);
        artRDM.setUni(portB[0], portB[1], deviceSettings.portBuni[0]);
        artRDM.setMerge(portB[0], portB[1], deviceSettings.portBmerge);

        // Lengthen or shorten our pixel strip & handle required Artnet ports
        if (newMode == TYPE_WS2812 && !doReboot) {
          // Get the new & old lengths of pixel strip
          uint16_t newLen = (json.containsKey("portBnumPix")) ? (uint16_t)json["portBnumPix"] : deviceSettings.portBnumPix;
          if (newLen > 680)
            newLen = 680;
          
          uint16_t oldLen = deviceSettings.portBnumPix;
          bool lenChanged = false;
  
          // If pixel size has changed
          if (newLen <= 680 && oldLen != newLen) {
            // Update our pixel strip
            deviceSettings.portBnumPix = newLen;
            pixDriver.updateStrip(1, deviceSettings.portBnumPix, deviceSettings.portBpixConfig);
  
            lenChanged = true;

            // If the old mode was pixel map then update the Artnet ports
            if (deviceSettings.portBpixMode == FX_MODE_PIXEL_MAP)
              updatePorts = true;
          }
            
          // If the old mode was 12 channel FX, update oldLen to represent the number of channels we used
          if (deviceSettings.portBpixMode == FX_MODE_12)
            oldLen = 12;
  
          // If our mode changes then update the Artnet ports
          if (deviceSettings.portBpixMode != (uint8_t)json["portBpixMode"])
            updatePorts = true;
  
          // Store the new pixel mode
          deviceSettings.portBpixMode = (uint8_t)json["portBpixMode"];
  
          // If our new mode is FX12 then we need 12 channels & store the start address
          if (deviceSettings.portBpixMode == FX_MODE_12) {
            if ((uint16_t)json["portBpixFXstart"] <= 501 && (uint16_t)json["portBpixFXstart"] > 0)
              deviceSettings.portBpixFXstart = (uint16_t)json["portBpixFXstart"];
            newLen = 12;
          }

          // If needed, open and close Artnet ports
          if (updatePorts) {
            for (uint8_t x = 1, y = 2; x < 4; x++, y++) {
              uint16_t c = (x * 170);
              if (newLen > c)
                portB[y] = artRDM.addPort(portB[0], x, deviceSettings.portBuni[x], TYPE_DMX_OUT, deviceSettings.portBmerge);
              else if (oldLen > c)
                artRDM.closePort(portB[0], portB[y]);
            }
          }
  
          // Set universe and merge settings (port 1 is done above for all port types)
          for (uint8_t x = 1, y = 2; x < 4; x++, y++) {
            if (newLen > (x * 170)) {
              artRDM.setUni(portB[0], portB[y], deviceSettings.portBuni[x]);
              artRDM.setMerge(portB[0], portB[y], deviceSettings.portBmerge);
            }
          }
        }
  
        artRDM.artPollReply();
  
        eepromSave();
        return true;
      }
      #endif
      break;

    case 6:     // Scenes
      // Not yet implemented
      
      return true;
      break;

    case 7:     // Firmware
      // Doesn't come here
      
      break;

    default:
      // Catch errors
      return false;
  }
}

void ajaxLoad(uint8_t page, JsonObject& jsonReply) {

  // Create the needed arrays here - doesn't work within the switch below
  JsonArray& ipAddress = jsonReply.createNestedArray("ipAddress");
  JsonArray& subAddress = jsonReply.createNestedArray("subAddress");
  JsonArray& gwAddress = jsonReply.createNestedArray("gwAddress");
  JsonArray& bcAddress = jsonReply.createNestedArray("bcAddress");
  JsonArray& portAuni = jsonReply.createNestedArray("portAuni");
  JsonArray& portBuni = jsonReply.createNestedArray("portBuni");
  JsonArray& portAsACNuni = jsonReply.createNestedArray("portAsACNuni");
  JsonArray& portBsACNuni = jsonReply.createNestedArray("portBsACNuni");
  JsonArray& dmxInBroadcast = jsonReply.createNestedArray("dmxInBroadcast");

  // Get MAC Address
  char MAC_char[30] = "";
  sprintf(MAC_char, "%02X", MAC_array[0]);
  for (int i = 1; i < 6; ++i)
    sprintf(MAC_char, "%s:%02X", MAC_char, MAC_array[i]);
  
  jsonReply["macAddress"] = String(MAC_char);

  switch (page) {
    case 1:     // Device Status
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portAuni");
      jsonReply.remove("portBuni");
      jsonReply.remove("portAsACNuni");
      jsonReply.remove("portBsACNuni");
      jsonReply.remove("dmxInBroadcast");
      
      jsonReply["nodeName"] = deviceSettings.nodeName;
      jsonReply["wifiStatus"] = wifiStatus;
      
      if (isHotspot) {
        jsonReply["ipAddressT"] = deviceSettings.hotspotIp.toString();
        jsonReply["subAddressT"] = deviceSettings.hotspotSubnet.toString();
      } else {
        jsonReply["ipAddressT"] = deviceSettings.ip.toString();
        jsonReply["subAddressT"] = deviceSettings.subnet.toString();
      }

      if (isHotspot && !deviceSettings.standAloneEnable) {
        jsonReply["portAStatus"] = "Disabled in hotspot mode";
        jsonReply["portBStatus"] = "Disabled in hotspot mode";
      } else {
        switch (deviceSettings.portAmode) {
          case TYPE_DMX_OUT:
            jsonReply["portAStatus"] = "DMX output";
            break;
  
          case TYPE_RDM_OUT:
            jsonReply["portAStatus"] = "DMX/RDM output";
            break;
  
          case TYPE_DMX_IN:
            jsonReply["portAStatus"] = "DMX input";
            break;
  
          case TYPE_WS2812:
            jsonReply["portAStatus"] = "WS2812 mode";
            break;
        }
        switch (deviceSettings.portBmode) {
          case TYPE_DMX_OUT:
            jsonReply["portBStatus"] = "DMX output";
            break;
  
          case TYPE_RDM_OUT:
            jsonReply["portBStatus"] = "DMX/RDM output";
            break;
  
          case TYPE_DMX_IN:
            jsonReply["portBStatus"] = "DMX input";
            break;
  
          case TYPE_WS2812:
            jsonReply["portBStatus"] = "WS2812 mode";
            break;
        }
      }
      
      jsonReply["sceneStatus"] = "Not outputting<br />0 Scenes Recorded<br />0 of 250KB used";
      jsonReply["firmwareStatus"] = FIRMWARE_VERSION;

      jsonReply["success"] = 1;
      break;

    case 2:     // Wifi
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portAuni");
      jsonReply.remove("portBuni");
      jsonReply.remove("portAsACNuni");
      jsonReply.remove("portBsACNuni");
      jsonReply.remove("dmxInBroadcast");
      
      jsonReply["wifiSSID"] = deviceSettings.wifiSSID;
      jsonReply["wifiPass"] = deviceSettings.wifiPass;
      jsonReply["hotspotSSID"] = deviceSettings.hotspotSSID;
      jsonReply["hotspotPass"] = deviceSettings.hotspotPass;
      jsonReply["hotspotDelay"] = deviceSettings.hotspotDelay;
      jsonReply["standAloneEnable"] = deviceSettings.standAloneEnable;
      
      jsonReply["success"] = 1;
      break;

    case 3:     // IP Address & Node Name
      jsonReply.remove("portAuni");
      jsonReply.remove("portBuni");
      jsonReply.remove("portAsACNuni");
      jsonReply.remove("portBsACNuni");
      jsonReply.remove("dmxInBroadcast");
      
      jsonReply["dhcpEnable"] = deviceSettings.dhcpEnable;

      for (uint8_t x = 0; x < 4; x++) {
        ipAddress.add(deviceSettings.ip[x]);
        subAddress.add(deviceSettings.subnet[x]);
        gwAddress.add(deviceSettings.gateway[x]);
        bcAddress.add(deviceSettings.broadcast[x]);
      }

      jsonReply["nodeName"] = deviceSettings.nodeName;
      jsonReply["longName"] = deviceSettings.longName;

      jsonReply["success"] = 1;
      break;

    case 4:     // Port A
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portBuni");
      jsonReply.remove("portBsACNuni");
      
      jsonReply["portAmode"] = deviceSettings.portAmode;

      // Only Artnet supported for receiving right now
      if (deviceSettings.portAmode == TYPE_DMX_IN)
        jsonReply["portAprot"] = PROT_ARTNET;
      else
        jsonReply["portAprot"] = deviceSettings.portAprot;
 
      jsonReply["portAmerge"] = deviceSettings.portAmerge;
      jsonReply["portAnet"] = deviceSettings.portAnet;
      jsonReply["portAsub"] = deviceSettings.portAsub;
      jsonReply["portAnumPix"] = deviceSettings.portAnumPix;
      
      jsonReply["portApixMode"] = deviceSettings.portApixMode;
      jsonReply["portApixFXstart"] = deviceSettings.portApixFXstart;

      for (uint8_t x = 0; x < 4; x++) {
        portAuni.add(deviceSettings.portAuni[x]);
        portAsACNuni.add(deviceSettings.portAsACNuni[x]);
        dmxInBroadcast.add(deviceSettings.dmxInBroadcast[x]);
      }

      jsonReply["success"] = 1;
      break;

    case 5:     // Port B
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portAuni");
      jsonReply.remove("portAsACNuni");
      
      jsonReply["portBmode"] = deviceSettings.portBmode;
      jsonReply["portBprot"] = deviceSettings.portBprot;
      jsonReply["portBmerge"] = deviceSettings.portBmerge;
      jsonReply["portBnet"] = deviceSettings.portBnet;
      jsonReply["portBsub"] = deviceSettings.portBsub;
      jsonReply["portBnumPix"] = deviceSettings.portBnumPix;
      
      jsonReply["portBpixMode"] = deviceSettings.portBpixMode;
      jsonReply["portBpixFXstart"] = deviceSettings.portBpixFXstart;

      for (uint8_t x = 0; x < 4; x++) {
        portBuni.add(deviceSettings.portBuni[x]);
        portBsACNuni.add(deviceSettings.portBsACNuni[x]);
        dmxInBroadcast.add(deviceSettings.dmxInBroadcast[x]);
      }
      
      jsonReply["success"] = 1;
      break;

    case 6:     // Scenes
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portAuni");
      jsonReply.remove("portBuni");
      jsonReply.remove("portAsACNuni");
      jsonReply.remove("portBsACNuni");
      jsonReply.remove("dmxInBroadcast");
      

      jsonReply["success"] = 1;
      break;

    case 7:     // Firmware
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portAuni");
      jsonReply.remove("portBuni");
      jsonReply.remove("portAsACNuni");
      jsonReply.remove("portBsACNuni");
      jsonReply.remove("dmxInBroadcast");
      
      jsonReply["firmwareStatus"] = FIRMWARE_VERSION;
      jsonReply["success"] = 1;
      break;

    default:
      jsonReply.remove("ipAddress");
      jsonReply.remove("subAddress");
      jsonReply.remove("gwAddress");
      jsonReply.remove("bcAddress");
      jsonReply.remove("portAuni");
      jsonReply.remove("portBuni");
      jsonReply.remove("portAsACNuni");
      jsonReply.remove("portBsACNuni");
      jsonReply.remove("dmxInBroadcast");
      
      jsonReply["success"] = 0;
      jsonReply["message"] = "Invalid or incomplete data received.";
  }
}

