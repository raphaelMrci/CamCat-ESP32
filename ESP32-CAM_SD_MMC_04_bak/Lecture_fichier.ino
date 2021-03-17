String Lecture_fichier(String Lire)
{
  String Obtenu;
  File Fichier=SD_MMC.open(Lire, FILE_READ);

  if (Fichier==0)
  {
    Serial.println("Fichier absent dans la carte SD");
  }

  byte Nombre_de_lettres=Fichier.available();

  while (Nombre_de_lettres!=0)
  {
    char Lettre=Fichier.read();
    Obtenu.concat(Lettre);
    Nombre_de_lettres=Fichier.available();
  }

  Fichier.close();
  return Obtenu;
}
