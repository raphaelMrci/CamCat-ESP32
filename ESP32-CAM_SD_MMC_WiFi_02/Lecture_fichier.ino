char* Lecture_fichier(String Lire)
{
  File Fichier=SD_MMC.open(Lire, FILE_READ);

  if (Fichier==0)
  {
    Serial.println("Fichier absent dans la carte SD");
  }

  String Lecture=Fichier.readString();

  byte Longueur=Lecture.length()+1;

  char* Obtenu;

  Lecture.toCharArray(Obtenu,Longueur);

  Fichier.close();

  Serial.println(Lecture);

  Serial.println(Longueur);

  return Obtenu;
}
