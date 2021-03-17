/*  // initialisation de la carte micro SD
  // en mode 1 bit: plus lent, mais libère des broches
  if (SD_MMC.begin("/sdcard",true)) { 
    uint8_t cardType = SD_MMC.cardType();
    if (cardType != CARD_NONE) {
      Serial.println("Carte SD Initialisee.");
      SD_en_place = true;
    }
  }
*/
#include "SD_MMC.h"
#include "WiFi.h"

#define PORT_LED_FLASH      4   // Numéro de port auquel est branchée la LED servant de flash.
  
String Identifiant;
String Mot_de_passe;

void setup()
{
  Serial.begin(9600);

  bool SD_en_place=SD_MMC.begin();

  if (SD_en_place==0)
  {
    Serial.println("Pas de carte SD en place");
    return;
  }

  Identifiant=Lecture_fichier("/ssid.txt");

  Mot_de_passe=Lecture_fichier("/passe.txt");

  Serial.println();

  Serial.println("Identifiant :");

  Serial.println(Identifiant);

  Serial.println("Mot de passe :");

  Serial.println(Mot_de_passe);

    // Variables de la fonction
    wl_status_t StatutConnexion_L; // Pour mémoriser l'état de la connexion
    pinMode(PORT_LED_FLASH, OUTPUT); // Initialisation en "sortie" de la broche d'E/S connectée au flash
//    WiFi.begin("Identifiant","Mot_de_passe"); // Tentative de connexion au point d'accès Wi-Fi
    WiFi.begin("Identifiant","20460701"); // Tentative de connexion au point d'accès Wi-Fi
//    WiFi.begin("No mind the dog, beware of owner","Mot_de_passe"); // Tentative de connexion au point d'accès Wi-Fi
//    WiFi.begin("No mind the dog, beware of owner","20460701"); // Tentative de connexion au point d'accès Wi-Fi
    StatutConnexion_L = WiFi.status(); // Lecture de l'état de la connexion et mémorisation dans la variable "StatutConnexion_L"
    while ((StatutConnexion_L != WL_NO_SSID_AVAIL)&&(StatutConnexion_L != WL_CONNECTED)&&(StatutConnexion_L != WL_CONNECT_FAILED))
    {
/*        digitalWrite(PORT_LED_FLASH, HIGH);
        delay(5);
        digitalWrite(PORT_LED_FLASH, LOW);*/
        delay(500);
        Serial.println("Patience !");
        StatutConnexion_L = WiFi.status(); // Lecture de l'état de la connexion et mémorisation dans la variable "StatutConnexion_L"
    }
    // Affichage du résultat de la tentative de connexion
    if (StatutConnexion_L == WL_CONNECTED)
    {
        Serial.println("Connection OK");
    }
    else if (StatutConnexion_L == WL_NO_SSID_AVAIL)
    {
        Serial.println("SSID introuvable");
    }
    else if (StatutConnexion_L == WL_CONNECT_FAILED)
    {
        Serial.println("Mot de passe KO");
    }
    else
    {
        Serial.println("Autre erreur");
    }
}
  
void loop()
{
      if(WiFi.status()==WL_CONNECTED)
    {  
        Serial.println("Connection OK");
        digitalWrite(PORT_LED_FLASH, HIGH);
        delay(20);
        digitalWrite(PORT_LED_FLASH, LOW);
        delay(1000);
    }
    else
    {
        // Pas de connexion internet
        Serial.println("Connection KO");
        digitalWrite(PORT_LED_FLASH, HIGH);
        delay(1000);
        digitalWrite(PORT_LED_FLASH, LOW);
        delay(20);
    }
}
