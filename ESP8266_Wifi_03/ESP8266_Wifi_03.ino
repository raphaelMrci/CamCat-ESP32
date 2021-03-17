#include <ESP8266WiFi.h>

//char ssid[]="No mind the dog, beware of owner";
//char password[]="20460701";

String ObtenuSsid="No mind the dog, beware of owner";
//String ObtenuSsid="Identifiant";

String ObtenuPassword="20460701";
//String ObtenuPassword="MotDePasse";

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
  while ((WiFi.status() != WL_CONNECTED)&&(WiFi.status() != WL_NO_SSID_AVAIL)&&(WiFi.status() != WL_CONNECT_FAILED))
  {
    delay(500);
    Serial.println("Patientez !");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connection OK");
    Serial.println("Connecté, IP address : ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
  }
  else if (WiFi.status() == WL_NO_SSID_AVAIL)
  {
    Serial.println("SSID introuvable");
    Serial.println("Connection KO");
  }
  else if (WiFi.status() == WL_CONNECT_FAILED)
  {
    Serial.println("Mot de passe KO");
    Serial.println("Connection KO");
  }
  else
  {
    Serial.println("Autre erreur");
    Serial.println("Connection KO");
  }
}

void loop()
{
}
