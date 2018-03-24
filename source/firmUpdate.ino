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



/* webFirmwareUpdate()
 *  display update status after firmware upload and restart
 */
const char PROGMEM unknownError[] = "{\"success\":0,\"message\":\"Unknown Error\"}";
const char PROGMEM successRestarting[] = "{\"success\":1,\"message\":\"Success: Device restarting\"}";
const char PROGMEM insufficientSpace[] = "{\"success\":0,\"message\":\"Insufficient space.\"}";
const char PROGMEM failedToSave[]= "{\"success\":0,\"message\":\"Failed to save\"}";
const char PROGMEM strConnection[] = "Connection";
const char PROGMEM strClose[] = "Close";
const char PROGMEM applicationJson[] = "application/json";
const char PROGMEM accessControlOrigin[] = "Access-Control-Allow-Origin";
void webFirmwareUpdate() {
  // Generate the webpage from the variables above
  // Send to the client
  webServer.sendHeader(strConnection, strClose);
  webServer.sendHeader(accessControlOrigin, "*");
  webServer.send(200, applicationJson, (Update.hasError()) ? unknownError : successRestarting);

  doReboot = true;
}



/* webFirmwareUpload()
 *  handle firmware upload and update
 */
void webFirmwareUpload() {
  String reply = "";
  HTTPUpload& upload = webServer.upload();
    
  if(upload.status == UPLOAD_FILE_START){
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if(!Update.begin(maxSketchSpace)){//start with max available size
      reply = insufficientSpace;
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      reply = failedToSave;
    }
    
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ //true to set the size to the current progress
      reply = successRestarting;
    } else {
      reply = unknownError;
    }
  }
  yield();
  
  // Send to the client
  if (reply.length() > 0) {
    webServer.sendHeader(strConnection, strClose);
    webServer.sendHeader(accessControlOrigin, "*");
    webServer.send(200, applicationJson, reply);
  }
}
