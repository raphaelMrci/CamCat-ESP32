#include <ESP8266WiFi.h>

char ssid[]="No mind the dog, beware of owner";
char password[]="20460701";

//char* ssid = "No mind the dog, beware of owner";  //(Initialement const char*) Mettre votre SSID Wifi
//char* password = "20460701";  //(Initialement const char*) Mettre votre mot de passe Wifi

void setup()
{
  Serial.begin(9600);
  Serial.println();

  WiFi.begin(ssid,password);

  Serial.println("En cours de connection");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Patientez !");
  }
  Serial.println();

  Serial.print("Connecté, IP address : ");
  Serial.println(WiFi.localIP());
}

void loop()
{
}
