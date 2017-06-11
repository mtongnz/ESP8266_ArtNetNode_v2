
/* webFirmwareUpdate()
 *  display update status after firmware upload and restart
 */
void webFirmwareUpdate() {
  // Generate the webpage from the variables above
  String fail = "{\"success\":0,\"message\":\"Unknown Error\"}";
  String ok = "{\"success\":1,\"message\":\"Success: Device restarting\"}";

  // Send to the client
  webServer.sendHeader("Connection", "close");
  webServer.sendHeader("Access-Control-Allow-Origin", "*");
  webServer.send(200, "application/json", (Update.hasError()) ? fail : ok);

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
      reply = "{\"success\":0,\"message\":\"Insufficient space.\"}";
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      reply = "{\"success\":0,\"message\":\"Failed to save\"}";
    }
    
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ //true to set the size to the current progress
      reply = "{\"success\":1,\"message\":\"Success: Device Restarting\"}";
    } else {
      reply = "{\"success\":0,\"message\":\"Unknown Error\"}";
    }
  }
  yield();
  
  // Send to the client
  if (reply.length() > 0) {
    webServer.sendHeader("Connection", "close");
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", reply);
  }
}
