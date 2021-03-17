#include "SD_MMC.h"
#include "WiFi.h"

#define Led_Rouge_Dessous 33   // Au cas où, port auquel est branchée la LED rouge qui est en-dessous (quand le module est sur son support de programmation).
byte status = WL_IDLE_STATUS;
  
//char* ssid = "No mind the dog, beware of owner";
//char* passe = "20460701";

void setup()
{
  Serial.begin(115200);

  pinMode(Led_Rouge_Dessous, OUTPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  bool SD_en_place=SD_MMC.begin("/sdcard",true);

  if (SD_en_place==0)
  {
    Serial.println("Pas de carte SD en place");
    return;
  }

  char* ssid=Lecture_fichier("/ssid.txt");

  char* passe=Lecture_fichier("/passe.txt");

  WiFi.begin(ssid,passe); // Tentative de connexion au point d'accès Wi-Fi

  wl_status_t Etat_de_la_connexion = WiFi.status(); // Lecture de l'état de la connexion et mémorisation

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WEP network, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, passe);

    // wait 10 seconds for connection:
    delay(10000);
  }
  
  // Affichage du résultat de la tentative de connexion
  if (Etat_de_la_connexion==WL_CONNECTED)
    {
       Serial.println("Connection OK");
    }
  else if (Etat_de_la_connexion==WL_NO_SSID_AVAIL)
    {
       Serial.println("SSID introuvable");
    }
  else if (Etat_de_la_connexion==WL_CONNECT_FAILED)
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
       delay(500);
    }
  else
    {
       Serial.println("Connection KO");
       delay(500);
    }
}
