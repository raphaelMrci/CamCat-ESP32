//based on https://github.com/yoursunny/esp32cam
#include <esp32cam.h>
#include <WebServer.h>
#include <WiFi.h>
#include "SD_MMC.h"

extern "C"
{
#include "crypto/base64.h"
}

const char *streamRealm = "ESP32-CAM, please log in!";
const char *authFailResponse = "Sorry, login failed!";

const char *streamPath = "/stream";

static auto hiRes = esp32cam::Resolution::find(800, 600);

const uint8_t jpgqal = 80;
const uint8_t fps = 10; //sets minimum delay between frames, HW limits of ESP32 allows about 12fps @ 800x600

WebServer server(80);

void changeUserAuth()
{
  //SD reading
  if (!SD_MMC.begin())
  {
    Serial.println("Failed to mount card");
    return;
  }

  //Get SD card identities
  String readUser = readFile("/.user");
  char streamUsername[readUser.length() + 1];
  readUser.toCharArray(streamUsername, readUser.length() + 1);

  String readPass = readFile("/.password");
  char streamPassword[readPass.length() + 1];
  readPass.toCharArray(streamPassword, readPass.length() + 1);

  //Authentication
  if (!server.authenticate(streamUsername, streamPassword))
  {
    Serial.println(F("STREAM auth required, sending request"));
    return server.requestAuthentication(BASIC_AUTH, streamRealm, authFailResponse);
  }

  if (server.hasArg("plain") == false)
  { //Check if body received

    server.send(200, "text/plain", "Body not received");
    return;
  }
  String brutText = server.arg("plain"); //TODO: Do with toCharArray()
  char newUserAndPass[brutText.length() + 1];
  brutText.toCharArray(newUserAndPass, brutText.length() + 1);
  
  Serial.println(newUserAndPass);

  char * token;
  token = strtok(newUserAndPass, ":");
  char * newUser = token;
  
  token = strtok(NULL, ":");
  char * newPass = token;
  
  while(token)
  {
    token = strtok(NULL,"|");
  }

  Serial.println(newUser);
  Serial.println(newPass);
  writeFile("/.user", newUser);
  writeFile("/.password", newPass);

  server.send(200, "text/plain", "Changes ok");
}

void handleMjpeg()
{
  //SD reading
  if (!SD_MMC.begin())
  {
    Serial.println("Failed to mount card");
    return;
  }

  //Get SD card identities
  String readUser = readFile("/.user");
  char streamUsername[readUser.length() + 1];
  readUser.toCharArray(streamUsername, readUser.length() + 1);

  String readPass = readFile("/.password");
  char streamPassword[readPass.length() + 1];
  readPass.toCharArray(streamPassword, readPass.length() + 1);

  //Authentication
  if (!server.authenticate(streamUsername, streamPassword))
  {
    Serial.println(F("STREAM auth required, sending request"));
    return server.requestAuthentication(BASIC_AUTH, streamRealm, authFailResponse);
  }

  if (!esp32cam::Camera.changeResolution(hiRes))
  {
    Serial.println(F("SET RESOLUTION FAILED"));
  }

  struct esp32cam::CameraClass::StreamMjpegConfig mjcfg;
  mjcfg.frameTimeout = 10000;
  mjcfg.minInterval = 1000 / fps;
  mjcfg.maxFrames = -1;
  Serial.println(String(F("STREAM BEGIN @ ")) + fps + F("fps (minInterval ") + mjcfg.minInterval + F("ms)"));
  WiFiClient client = server.client();
  auto startTime = millis();
  int res = esp32cam::Camera.streamMjpeg(client, mjcfg);
  if (res <= 0)
  {
    Serial.printf("STREAM ERROR %d\n", res);
    return;
  }
  auto duration = millis() - startTime;
  Serial.printf("STREAM END %dfrm %0.2ffps\n", res, 1000.0 * res / duration);
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(jpgqal);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? F("CAMERA OK") : F("CAMERA FAIL"));
  }

  Serial.println(String(F("JPEG quality: ")) + jpgqal);
  Serial.println(String(F("Framerate: ")) + fps);

  //SD reading
  if (!SD_MMC.begin())
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
  Serial.print(WiFi.localIP());
  Serial.println(streamPath);

  server.on(streamPath, handleMjpeg);
  server.on("/auth/edit", changeUserAuth);

  server.begin();
}

void loop()
{
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

void writeFile(const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = SD_MMC.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
}
