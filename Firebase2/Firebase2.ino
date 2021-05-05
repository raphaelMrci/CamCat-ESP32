const char IID[] = "CI5L8P542X35DS45";

#include <WiFi.h>
#include "SD_MMC.h"
#include <EEPROM.h>
#include "ESPAsyncWebServer.h"
//#include "firestore.h"

//#define FIRESTORE_FIREBASE_PROJECT_ID "camcat-be48c"
//#define FIRESTORE_FIREBASE_API_KEY "AIzaSyCqROWiVuc4YNM0Y2rWpxa2NA4unGDTmrc"

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


void setup()
{
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
                  Serial.write(EEPROM.read(i));
                  //Serial.print(" ");
                  i++;
                }
                Serial.println();
              } else {
                for (size_t i = 0; i < len; i++) {
                  Serial.print((char)data[i]);
                  EEPROM.write(i, (int)data[i]);
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
        Serial.println("Erased successfully");
        break;
      default:
        printCommands();
    }
  } else if (Serial.available() > 0){
    while (Serial.available()>0){ Serial.read(); }
  }
}
