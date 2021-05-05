const char IID[] = "CI5L8P542X35DS45";

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

//CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

AsyncWebServer server(80);
bool userConnected = false;
char * UID;

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
String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

String Photo2Base64() {
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();  
    if(!fb) {
      Serial.println("Camera capture failed");
      return "";
    }
  
    String imageFile = "data:image/jpeg;base64,";
    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];
    for (int i=0;i<fb->len;i++) {
      base64_encode(output, (input++), 3);
      if (i%3==0) imageFile += urlencode(String(output));
    }

    esp_camera_fb_return(fb);
    
    return imageFile;
}



void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  checkUser();
camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
    if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);  // VGA|CIF|QVGA|HQVGA|QQVGA   ( UXGA? SXGA? XGA? SVGA? )

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
              //return "CamCat-THE_DEVICE_ID-STATUS"
              String response = "CamCat-";
              response += IID;
              response += "-";
              if (userConnected)
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
              if (userConnected){
                request->send(403, "text/plain", "CamCat already assiociated to an account.");

                Serial.print("Try to associate to the CamCat even if it is already associated to an account: ");
                Serial.println();
              } else {
                for (size_t i = 0; i < len; i++) {
                  Serial.print((char)data[i]);
                  EEPROM.write(i, (int)data[i]);
                }
                Serial.println();
                EEPROM.commit();
                checkUser();
                request->send(200, "text/plain", "CamCat is assiociated successfully.");
              }
            });

  server.begin();
  Serial.println("Server started...");
}

void checkUser(){
  userConnected = EEPROM.read(0) != 255;
  int i = 0;
  
  while (EEPROM.read(i) != 255){ i++; }

  char chars[i];

  i=0;

  Serial.println();
  while (EEPROM.read(i) != 255){
    chars[i] = (char) EEPROM.read(i);
    i++;
  }
  UID = chars;
  Serial.print("User ID:");
  Serial.println(UID);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (userConnected){
      HTTPClient http;
      checkUser();

      http.begin("https://camcat.herokuapp.com/update");
      http.addHeader("Content-Type", "application/json");
      http.addHeader("X-camcat-user", UID);
      http.addHeader("X-camID", IID);

      int httpResponseCode = http.POST("{\"photo\":\"" + Photo2Base64() + "\"}");

      if (httpResponseCode>0){
        String response = http.getString();

         Serial.println(httpResponseCode);
         Serial.println(response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
  }
  delay(3000);
}
