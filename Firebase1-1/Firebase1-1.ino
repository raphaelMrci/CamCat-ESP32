const char IID[] = "CI5L8P542X35DS45";
char * anotherName = "";
bool userConnected = false;

#include <WiFi.h>
#include "SD_MMC.h"
#include <EEPROM.h>
#include "ESPAsyncWebServer.h"
#include <HTTPClient.h>
#include "esp_camera.h"
#include "Base64.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define EEPROM_SIZE 512

AsyncWebServer server(80);

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

void printCommands(){
  Serial.println("---------------------------");
  Serial.println("Commands:\n");
  Serial.println("r\t=\tReset EEPROM memory.");
}

void checkUser(){
  userConnected = EEPROM.read(0) != 255;
  int i = 0;
  
  while (EEPROM.read(i) != 255){ i++; }

  char chars[i+1];

  i=0;

  while (EEPROM.read(i) < 127){
    chars[i] = (char) EEPROM.read(i);
    i++;
  }
  anotherName = chars;
  Serial.print("User ID:");
  Serial.print(anotherName);
  Serial.println();
  Serial.println(userConnected);
}


void setup()
{
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  checkUser();
  checkUser();
  checkUser();

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

  checkUser();

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

  Serial.println(anotherName);

  Serial.print(F("\nCONNECTED!\nhttp://"));
  Serial.println(WiFi.localIP());

  server.on("/test",
            HTTP_GET,
            [](AsyncWebServerRequest *request) {
              String response = "CamCat-";
              response += IID;
              response += "-";
              if (EEPROM.read(0) != 255)
              {
                response += "CLOSED";
              }
              else
              {
                response += "OPEN";
              }
              Serial.println(response);
              request->send(200, "text/plain", response);
            });

  server.on("/camcat",
            HTTP_POST, 
            [](AsyncWebServerRequest *request) {},
            NULL,
            [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
              if (EEPROM.read(0) != 255){
                request->send(403, "text/plain", "CamCat already assiociated to an account.");

                Serial.print("Try to associate to the CamCat even if it is already associated to an account: ");
                int i = 0;

                while (EEPROM.read(i) != 255){
                  checkUser();
                  //Serial.print(" ");
                  i++;
                }
                Serial.println();
              } else {
                for (size_t i = 0; i < len+1; i++) {
                  EEPROM.write(i, data[i]);
                }
                Serial.println();
                EEPROM.commit();
                request->send(200, "text/plain", "CamCat is assiociated successfully.");
              }
            });

  server.begin();
  Serial.println("Server started...");
}

void loop() {
  if (Serial.available() == 1){
    char command = Serial.read();

    switch (command) {
      case 'r':
        Serial.println("Erasing EEPROM memory...");
        for (int i = 0; i < EEPROM_SIZE; i++) {
          EEPROM.write(i, 255);
        }
        EEPROM.commit();
        checkUser();
        Serial.println("Erased successfully");
        break;
      default:
        printCommands();
    }
  } else if (Serial.available() > 0){
    while (Serial.available()>0){ Serial.read(); }
  }
  
  if (userConnected){
    Serial.println(anotherName);
  }
  delay(1000);
}
