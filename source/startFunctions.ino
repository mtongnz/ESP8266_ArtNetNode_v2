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


void doNodeReport() {
  if (nextNodeReport > millis())
    return;
  
  char c[ARTNET_NODE_REPORT_LENGTH];

  if (nodeErrorTimeout > millis())
    nextNodeReport = millis() + 2000;
  else
    nextNodeReport = millis() + 5000;
  
  if (nodeError[0] != '\0' && !nodeErrorShowing && nodeErrorTimeout > millis()) {
    
    nodeErrorShowing = true;
    strcpy(c, nodeError);
    
  } else {
    nodeErrorShowing = false;
    
    strcpy(c, "OK: PortA:");

    switch (deviceSettings.portAmode) {
      case TYPE_DMX_OUT:
        sprintf(c, "%s DMX Out", c);
        break;
      
      case TYPE_RDM_OUT:
        sprintf(c, "%s RDM Out", c);
        break;
      
      case TYPE_DMX_IN:
        sprintf(c, "%s DMX In", c);
        break;
      
      case TYPE_WS2812:
        if (deviceSettings.portApixMode == FX_MODE_12)
            sprintf(c, "%s 12chan", c);
          sprintf(c, "%s WS2812 %ipixels", c, deviceSettings.portAnumPix);
        break;
    }
  
    #ifndef ONE_PORT
      sprintf(c, "%s. PortB:", c);
      
      switch (deviceSettings.portBmode) {
        case TYPE_DMX_OUT:
          sprintf(c, "%s DMX Out", c);
          break;
        
        case TYPE_RDM_OUT:
          sprintf(c, "%s RDM Out", c);
          break;
        
        case TYPE_WS2812:
          if (deviceSettings.portBpixMode == FX_MODE_12)
            sprintf(c, "%s 12chan", c);
          sprintf(c, "%s WS2812 %ipixels", c, deviceSettings.portBnumPix);
          break;
      }
    #endif
  }
  
  artRDM.setNodeReport(c, ARTNET_RC_POWER_OK);
}

void portSetup() {
  if (deviceSettings.portAmode == TYPE_DMX_OUT || deviceSettings.portAmode == TYPE_RDM_OUT) {
    #ifndef ESP_01
      setStatusLed(STATUS_LED_A, BLUE);
    #endif
    
    dmxA.begin(DMX_DIR_A, artRDM.getDMX(portA[0], portA[1]));
    if (deviceSettings.portAmode == TYPE_RDM_OUT && !dmxA.rdmEnabled()) {
      dmxA.rdmEnable(ESTA_MAN, ESTA_DEV);
      dmxA.rdmSetCallBack(rdmReceivedA);
      dmxA.todSetCallBack(sendTodA);
    }

  } else if (deviceSettings.portAmode == TYPE_DMX_IN) {
    #ifndef ESP_01
      setStatusLed(STATUS_LED_A, CYAN);
    #endif
    
    dmxA.begin(DMX_DIR_A, artRDM.getDMX(portA[0], portA[1]));
    dmxA.dmxIn(true);
    dmxA.setInputCallback(dmxIn);

    dataIn = (byte*) os_malloc(sizeof(byte) * 512);
    memset(dataIn, 0, 512);

  } else if (deviceSettings.portAmode == TYPE_WS2812) {
    #ifndef ESP_01
      setStatusLed(STATUS_LED_A, GREEN);
    #endif
    
    digitalWrite(DMX_DIR_A, HIGH);
    pixDriver.setStrip(0, DMX_TX_A, deviceSettings.portAnumPix, deviceSettings.portApixConfig);
  }
  
  #ifndef ONE_PORT
    if (deviceSettings.portBmode == TYPE_DMX_OUT || deviceSettings.portBmode == TYPE_RDM_OUT) {
      setStatusLed(STATUS_LED_B, BLUE);
      
      dmxB.begin(DMX_DIR_B, artRDM.getDMX(portB[0], portB[1]));
      if (deviceSettings.portBmode == TYPE_RDM_OUT && !dmxB.rdmEnabled()) {
        dmxB.rdmEnable(ESTA_MAN, ESTA_DEV);
        dmxB.rdmSetCallBack(rdmReceivedB);
        dmxB.todSetCallBack(sendTodB);
      }
      
    } else if (deviceSettings.portBmode == TYPE_WS2812) {
      setStatusLed(STATUS_LED_B, GREEN);
      
      digitalWrite(DMX_DIR_B, HIGH);
      pixDriver.setStrip(1, DMX_TX_B, deviceSettings.portBnumPix, deviceSettings.portBpixConfig);
    }
  #endif

  pixDriver.allowInterruptSingle = WS2812_ALLOW_INT_SINGLE;
  pixDriver.allowInterruptDouble = WS2812_ALLOW_INT_DOUBLE;
}

void artStart() {
  // Initialise out ArtNet
  if (isHotspot)
    artRDM.init(deviceSettings.hotspotIp, deviceSettings.hotspotSubnet, true, deviceSettings.nodeName, deviceSettings.longName, ARTNET_OEM, ESTA_MAN, MAC_array);
  else
    artRDM.init(deviceSettings.ip, deviceSettings.subnet, deviceSettings.dhcpEnable, deviceSettings.nodeName, deviceSettings.longName, ARTNET_OEM, ESTA_MAN, MAC_array);

  // Set firmware
  artRDM.setFirmwareVersion(ART_FIRM_VERSION);

  // Add Group
  portA[0] = artRDM.addGroup(deviceSettings.portAnet, deviceSettings.portAsub);
  
  bool e131 = (deviceSettings.portAprot == PROT_ARTNET_SACN) ? true : false;

  // WS2812 uses TYPE_DMX_OUT - the rest use the value assigned
  if (deviceSettings.portAmode == TYPE_WS2812)
    portA[1] = artRDM.addPort(portA[0], 0, deviceSettings.portAuni[0], TYPE_DMX_OUT, deviceSettings.portAmerge);
  else
    portA[1] = artRDM.addPort(portA[0], 0, deviceSettings.portAuni[0], deviceSettings.portAmode, deviceSettings.portAmerge);

  artRDM.setE131(portA[0], portA[1], e131);
  artRDM.setE131Uni(portA[0], portA[1], deviceSettings.portAsACNuni[0]);
  
  // Add extra Artnet ports for WS2812
  if (deviceSettings.portAmode == TYPE_WS2812 && deviceSettings.portApixMode == FX_MODE_PIXEL_MAP) {
    if (deviceSettings.portAnumPix > 170) {
      portA[2] = artRDM.addPort(portA[0], 1, deviceSettings.portAuni[1], TYPE_DMX_OUT, deviceSettings.portAmerge);
      
      artRDM.setE131(portA[0], portA[2], e131);
      artRDM.setE131Uni(portA[0], portA[2], deviceSettings.portAsACNuni[1]);
    }
    if (deviceSettings.portAnumPix > 340) {
      portA[3] = artRDM.addPort(portA[0], 2, deviceSettings.portAuni[2], TYPE_DMX_OUT, deviceSettings.portAmerge);
      
      artRDM.setE131(portA[0], portA[3], e131);
      artRDM.setE131Uni(portA[0], portA[3], deviceSettings.portAsACNuni[2]);
    }
    if (deviceSettings.portAnumPix > 510) {
      portA[4] = artRDM.addPort(portA[0], 3, deviceSettings.portAuni[3], TYPE_DMX_OUT, deviceSettings.portAmerge);
      
      artRDM.setE131(portA[0], portA[4], e131);
      artRDM.setE131Uni(portA[0], portA[4], deviceSettings.portAsACNuni[3]);
    }
  }


  #ifndef ONE_PORT
    // Add Group
    portB[0] = artRDM.addGroup(deviceSettings.portBnet, deviceSettings.portBsub);
    e131 = (deviceSettings.portBprot == PROT_ARTNET_SACN) ? true : false;
    
    // WS2812 uses TYPE_DMX_OUT - the rest use the value assigned
    if (deviceSettings.portBmode == TYPE_WS2812)
      portB[1] = artRDM.addPort(portB[0], 0, deviceSettings.portBuni[0], TYPE_DMX_OUT, deviceSettings.portBmerge);
    else
      portB[1] = artRDM.addPort(portB[0], 0, deviceSettings.portBuni[0], deviceSettings.portBmode, deviceSettings.portBmerge);

    artRDM.setE131(portB[0], portB[1], e131);
    artRDM.setE131Uni(portB[0], portB[1], deviceSettings.portBsACNuni[0]);
  
    // Add extra Artnet ports for WS2812
    if (deviceSettings.portBmode == TYPE_WS2812 && deviceSettings.portBpixMode == FX_MODE_PIXEL_MAP) {
      if (deviceSettings.portBnumPix > 170) {
        portB[2] = artRDM.addPort(portB[0], 1, deviceSettings.portBuni[1], TYPE_DMX_OUT, deviceSettings.portBmerge);
        
        artRDM.setE131(portB[0], portB[2], e131);
        artRDM.setE131Uni(portB[0], portB[2], deviceSettings.portBsACNuni[1]);
      }
      if (deviceSettings.portBnumPix > 340) {
        portB[3] = artRDM.addPort(portB[0], 2, deviceSettings.portBuni[2], TYPE_DMX_OUT, deviceSettings.portBmerge);
        
        artRDM.setE131(portB[0], portB[3], e131);
        artRDM.setE131Uni(portB[0], portB[3], deviceSettings.portBsACNuni[2]);
      }
      if (deviceSettings.portBnumPix > 510) {
        portB[4] = artRDM.addPort(portB[0], 3, deviceSettings.portBuni[3], TYPE_DMX_OUT, deviceSettings.portBmerge);
        
        artRDM.setE131(portB[0], portB[4], e131);
        artRDM.setE131Uni(portB[0], portB[4], deviceSettings.portBsACNuni[3]);
      }
    }
  #endif

  // Add required callback functions
  artRDM.setArtDMXCallback(dmxHandle);
  artRDM.setArtRDMCallback(rdmHandle);
  artRDM.setArtSyncCallback(syncHandle);
  artRDM.setArtIPCallback(ipHandle);
  artRDM.setArtAddressCallback(addressHandle);
  artRDM.setTODRequestCallback(todRequest);
  artRDM.setTODFlushCallback(todFlush);


  switch (resetInfo.reason) {
    case REASON_DEFAULT_RST:  // normal start
    case REASON_EXT_SYS_RST:
    case REASON_SOFT_RESTART:
      artRDM.setNodeReport("OK: Device started", ARTNET_RC_POWER_OK);
      nextNodeReport = millis() + 4000;
      break;
      
    case REASON_WDT_RST:
      artRDM.setNodeReport("ERROR: (HWDT) Unexpected device restart", ARTNET_RC_POWER_FAIL);
      strcpy(nodeError, "Restart error: HWDT");
      nextNodeReport = millis() + 10000;
      nodeErrorTimeout = millis() + 30000;
      break;
    case REASON_EXCEPTION_RST:
      artRDM.setNodeReport("ERROR: (EXCP) Unexpected device restart", ARTNET_RC_POWER_FAIL);
      strcpy(nodeError, "Restart error: EXCP");
      nextNodeReport = millis() + 10000;
      nodeErrorTimeout = millis() + 30000;
      break;
    case REASON_SOFT_WDT_RST:
      artRDM.setNodeReport("ERROR: (SWDT) Unexpected device restart", ARTNET_RC_POWER_FAIL);
      strcpy(nodeError, "Error on Restart: SWDT");
      nextNodeReport = millis() + 10000;
      nodeErrorTimeout = millis() + 30000;
      break;
    case REASON_DEEP_SLEEP_AWAKE:
      // not used
      break;
  }
  
  // Start artnet
  artRDM.begin();

  yield();
}

void webStart() {
  webServer.on("/", [](){
    artRDM.pause();
    webServer.send_P(200, typeHTML, mainPage);
    webServer.sendHeader("Connection", "close");
    yield();
    artRDM.begin();
  });
  
  webServer.on("/style.css", [](){
    artRDM.pause();

    File f = SPIFFS.open("/style.css", "r");

    // If no style.css in SPIFFS, send default
    if (!f)
      webServer.send_P(200, typeCSS, css);
    else
      size_t sent = webServer.streamFile(f, typeCSS);
    
    f.close();
    webServer.sendHeader("Connection", "close");
    
    yield();
    artRDM.begin();
  });
  
  webServer.on("/ajax", HTTP_POST, ajaxHandle);
  
  webServer.on("/upload", HTTP_POST, webFirmwareUpdate, webFirmwareUpload);

  webServer.on("/style", [](){
    webServer.send_P(200, typeHTML, cssUploadPage);
    webServer.sendHeader("Connection", "close");
  });
  
  webServer.on("/style_delete", [](){
    if (SPIFFS.exists("/style.css"))
      SPIFFS.remove("/style.css");
        
    webServer.send(200, "text/plain", "style.css deleted.  The default style is now in use.");
    webServer.sendHeader("Connection", "close");
  });

  webServer.on("/style_upload", HTTP_POST, [](){
    webServer.send(200, "text/plain", "Upload successful!");
  }, [](){
    HTTPUpload& upload = webServer.upload();
    
    if(upload.status == UPLOAD_FILE_START){
      String filename = upload.filename;
      if(!filename.startsWith("/")) filename = "/"+filename;
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
      
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize);
        
    } else if(upload.status == UPLOAD_FILE_END){
      if(fsUploadFile) {
        fsUploadFile.close();
        
        if (upload.filename != "/style.css")
          SPIFFS.rename(upload.filename, "/style.css");
      }
    }
  });
  
  webServer.onNotFound([]() {
    webServer.send(404, "text/plain", "Page not found");
  });
  
  webServer.begin();
  
  yield();
}

void wifiStart() {
  // If it's the default WiFi SSID, make it unique
  if (strcmp(deviceSettings.hotspotSSID, "espArtNetNode") == 0 || deviceSettings.hotspotSSID[0] == '\0')
    sprintf(deviceSettings.hotspotSSID, "espArtNetNode_%05u", (ESP.getChipId() & 0xFF));
  
  if (deviceSettings.standAloneEnable) {
    startHotspot();

    deviceSettings.ip = deviceSettings.hotspotIp;
    deviceSettings.subnet = deviceSettings.hotspotSubnet;
    deviceSettings.broadcast = {~deviceSettings.subnet[0] | (deviceSettings.ip[0] & deviceSettings.subnet[0]), ~deviceSettings.subnet[1] | (deviceSettings.ip[1] & deviceSettings.subnet[1]), ~deviceSettings.subnet[2] | (deviceSettings.ip[2] & deviceSettings.subnet[2]), ~deviceSettings.subnet[3] | (deviceSettings.ip[3] & deviceSettings.subnet[3])};
  
    return;
  }
  
  if (deviceSettings.wifiSSID[0] != '\0') {
    WiFi.begin(deviceSettings.wifiSSID, deviceSettings.wifiPass);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(deviceSettings.nodeName);
    
    unsigned long endTime = millis() + (deviceSettings.hotspotDelay * 1000);

    if (deviceSettings.dhcpEnable) {
      while (WiFi.status() != WL_CONNECTED && endTime > millis())
        yield();

      if (millis() >= endTime)
        startHotspot();
      
      deviceSettings.ip = WiFi.localIP();
      deviceSettings.subnet = WiFi.subnetMask();

      if (deviceSettings.gateway == INADDR_NONE)
        deviceSettings.gateway = WiFi.gatewayIP();
      
      deviceSettings.broadcast = {~deviceSettings.subnet[0] | (deviceSettings.ip[0] & deviceSettings.subnet[0]), ~deviceSettings.subnet[1] | (deviceSettings.ip[1] & deviceSettings.subnet[1]), ~deviceSettings.subnet[2] | (deviceSettings.ip[2] & deviceSettings.subnet[2]), ~deviceSettings.subnet[3] | (deviceSettings.ip[3] & deviceSettings.subnet[3])};
    } else
      WiFi.config(deviceSettings.ip, deviceSettings.gateway, deviceSettings.subnet);

    //sprintf(wifiStatus, "Wifi connected.  Signal: %ld<br />SSID: %s", WiFi.RSSI(), deviceSettings.wifiSSID);
    sprintf(wifiStatus, "Wifi connected.<br />SSID: %s", deviceSettings.wifiSSID);
    WiFi.macAddress(MAC_array);
    
  } else
    startHotspot();
  
  yield();
}

void startHotspot() {
  yield();
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(deviceSettings.hotspotSSID, deviceSettings.hotspotPass);
  WiFi.softAPConfig(deviceSettings.hotspotIp, deviceSettings.hotspotIp, deviceSettings.hotspotSubnet);

  sprintf(wifiStatus, "No Wifi. Hotspot started.<br />\nHotspot SSID: %s", deviceSettings.hotspotSSID);
  WiFi.macAddress(MAC_array);

  isHotspot = true;
  
  if (deviceSettings.standAloneEnable)
    return;
  
  webStart();

  unsigned long endTime = millis() + 30000;

  // Stay here if not in stand alone mode - no dmx or artnet
  while (endTime > millis() || wifi_softap_get_station_num() > 0) {
    webServer.handleClient();
    yield();
  }

  ESP.restart();
  isHotspot = false;
}
