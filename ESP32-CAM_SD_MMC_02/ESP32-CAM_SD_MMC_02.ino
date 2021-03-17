#include "SD_MMC.h"
  
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

  File Fichier=SD_MMC.open("/SSID.txt", FILE_READ);

  if (Fichier==0)
  {
    Serial.println("Pas de fichier SSID.txt dans la carte SD");
    return;
  }

  byte Nombre_de_lettres=Fichier.available();

  while (Nombre_de_lettres!=0)
  {
    char Lettre=Fichier.read();
//    Serial.println(Lettre);
    Identifiant.concat(Lettre);
    Nombre_de_lettres=Fichier.available();
  }

  Fichier.close();

  Fichier=SD_MMC.open("/Passe.txt", FILE_READ);

  if (Fichier==0)
  {
    Serial.println("Pas de fichier Passe.txt dans la carte SD");
    return;
  }

  Nombre_de_lettres=Fichier.available();

  while (Nombre_de_lettres!=0)
  {
    char Lettre=Fichier.read();
//    Serial.println(Lettre);
    Mot_de_passe.concat(Lettre);
    Nombre_de_lettres=Fichier.available();
  }

  Fichier.close();

  Serial.println();

  Serial.println(Identifiant);

  Serial.println(Mot_de_passe);

}
  
void loop()
{
}
