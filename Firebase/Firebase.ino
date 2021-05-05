const char IID[] = "CI5L8P542X35DS45";

#include <WiFi.h>
#include <WebServer.h>
#include "SD_MMC.h"
#include <EEPROM.h>

#define EEPROM_SIZE 512

WebServer server(80);

void handleTest(){
  String response = "CamCat-";
  response += IID;
  response += "-";
  if (EEPROM.read(0) != 0){
    response += "CLOSED";
  } else {
    response += "OPEN";
  }
  Serial.println(response);
  server.send(200, "text/plain", response);
}

void handleConnect(){
  if (EEPROM.read(0) != 0){
    
  } else {
    send(403, "text/plain", "CamCat already assiociated to an account.");
  }
}

void handleDelete(){
  //TODO: Clear EEPROM Memory
  for (byte i = 511; i<0; i--){
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void setup() {
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  
  if (!SD_MMC.begin("", true))
    {
      Serial.println("Failed to mount card");
      return;
    }
    String readSsid = readFile("/ssid.txt");
    char ssid[readSsid.length() + 1];
    readSsid.toCharArray(ssid, readSsid.length() + 1);
  
    String readPassword = readFile("/password.txt");
    char password[readPassword.length() + 1];
    readPassword.toCharArray(password, readPassword.length() + 1);
  
    Serial.println(ssid);
    Serial.println(password);
  
    //WiFi connection
    Serial.print(F("Connecting to WiFi"));
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(F("."));
      delay(500);
    }
  
    Serial.print(F("\nCONNECTED!\nhttp://"));
    Serial.println(WiFi.localIP());

    server.on("/test",handleTest);
    server.on("/camcat", handleConnect);
    server.begin();
    Serial.println("Server started...");

  /*
  if (EEPROM.read(0) == null){
    //Store ID on EEPROM memory
    for(byte i = 0; i < sizeof(IID) - 1; i++){
      EEPROM.write(i, (int)IID[i]);
    }
  }
  */
}

void loop() {
  server.handleClient();
}

String readFile(String path)
{
  File file = SD_MMC.open(path, FILE_READ);

  if (!file)
  {
    Serial.println("Opening file to read failed");
  }

  String reading = file.readString();

  file.close();
  return reading;
}
