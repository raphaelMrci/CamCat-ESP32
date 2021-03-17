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
  
void setup()
{
  Serial.begin(9600);

  bool SD_en_place=SD_MMC.begin();

  if (SD_en_place==0)
  {
    Serial.println("Pas de carte SD en place");
    return;
  }

  String Identifiant=Lecture_fichier("/ssid.txt");

  String Mot_de_passe=Lecture_fichier("/passe.txt");

  Serial.println();

  Serial.println("Identifiant :");

  Serial.println(Identifiant);

  Serial.println("Mot de passe :");

  Serial.println(Mot_de_passe);
}
  
void loop()
{
}
