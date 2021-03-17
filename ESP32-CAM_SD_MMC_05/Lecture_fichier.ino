String Lecture_fichier(String Lire)
{
  File Fichier=SD_MMC.open(Lire, FILE_READ);

  if (Fichier==0)
  {
    Serial.println("Fichier absent dans la carte SD");
  }

  String Obtenu=Fichier.readString();

  byte Longueur=Obtenu.length()+1;

  Serial.println(Longueur);

  Obtenu.toCharArray(IdentifiantChar,Longueur);

  Fichier.close();
  return Obtenu;
}
