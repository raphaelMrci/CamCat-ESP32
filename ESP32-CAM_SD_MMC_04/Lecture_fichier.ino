String Lecture_fichier(String Lire)
{
  File Fichier=SD_MMC.open(Lire, FILE_READ);

  if (Fichier==0)
  {
    Serial.println("Fichier absent dans la carte SD");
  }

  String Obtenu=Fichier.readString();

  Fichier.close();
  return Obtenu;
}
