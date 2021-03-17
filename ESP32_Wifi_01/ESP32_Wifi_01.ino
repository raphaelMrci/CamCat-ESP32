#include "WiFi.h"

//char ssid[]="No mind the dog, beware of owner";
//char password[]="20460701";

String ObtenuSsid="No mind the dog, beware of owner";
String ObtenuPassword="20460701";

char* ssid; //="No mind the dog, beware of owner";  //(Initialement const char*) Mettre votre SSID Wifi
char* password; //="20460701";  //(Initialement const char*) Mettre votre mot de passe Wifi

byte Longueur;

void setup()
{
  Serial.begin(9600);

  Longueur=ObtenuSsid.length()+1;

  ObtenuSsid.toCharArray(ssid,Longueur);

  Serial.println(Longueur);

  Serial.println(ssid);

  Longueur=ObtenuPassword.length()+1;

  ObtenuPassword.toCharArray(password,Longueur);

  Serial.println(Longueur);

  Serial.println(password);

  WiFi.begin(ssid,password);

  Serial.println("En cours de connection");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Patientez !");
  }
  Serial.println();

  Serial.println("Connecté, IP address : ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void loop()
{
}
