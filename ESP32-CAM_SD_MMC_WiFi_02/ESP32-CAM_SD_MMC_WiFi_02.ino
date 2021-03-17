#include "SD_MMC.h"
#include "WiFi.h"

#define Led_Rouge_Dessous 33   // Au cas où, port auquel est branchée la LED rouge qui est en-dessous (quand le module est sur son support de programmation).

void setup()
{
  Serial.begin(9600);

  pinMode(Led_Rouge_Dessous, OUTPUT);

  bool SD_en_place=SD_MMC.begin();

  if (SD_en_place==0)
  {
    Serial.println("Pas de carte SD en place");
    return;
  }

  char* ssid=Lecture_fichier("/ssid.txt");

  char* passe=Lecture_fichier("/password.txt");

  Serial.println(ssid);
  Serial.println(passe);

  WiFi.begin(ssid,passe); // Tentative de connexion au point d'accès Wi-Fi

  wl_status_t Etat_de_la_connexion = WiFi.status(); // Lecture de l'état de la connexion et mémorisation

  while ((Etat_de_la_connexion!=WL_NO_SSID_AVAIL)&&(Etat_de_la_connexion!=WL_CONNECTED)&&(Etat_de_la_connexion!=WL_CONNECT_FAILED))
    {
       delay(500);
       Serial.println("Patience !");
       Etat_de_la_connexion = WiFi.status(); // Lecture cyclique de l'état de la connexion et mémorisation
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
