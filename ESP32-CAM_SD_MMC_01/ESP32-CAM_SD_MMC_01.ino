#include "SD_MMC.h"
 
void setup()
{
    Serial.begin(9600);
     
    if(!SD_MMC.begin())
    {
      Serial.println("Failed to mount card");
      return;
    }

    uint8_t cardType = SD_MMC.cardType();
 
    Serial.print("\nCard Type: ");
     
    if(cardType == CARD_NONE)
    {
      Serial.println("No card attached");
      return;
    }
    if(cardType == CARD_MMC)
    {
      Serial.println("MMC");
    }
    else if(cardType == CARD_SD)
    {
      Serial.println("SDSC");
    }
    else if(cardType == CARD_SDHC)
    {
      Serial.println("SDHC");
    }
    else
    {
      Serial.println("UNKNOWN");
    }
 
    uint64_t cardSize = SD_MMC.cardSize();
    int cardSizeInMB = cardSize/(1024 * 1024);
     
    Serial.print("Card size: ");
    Serial.println(cardSizeInMB);
 
    uint64_t bytesAvailable = SD_MMC.totalBytes(); 
    int bytesAvailableInMB = bytesAvailable/(1024 * 1024);
 
    Serial.print("MB available: ");
    Serial.println(bytesAvailableInMB);
 
    uint64_t bytesUsed = SD_MMC.usedBytes();
    int bytesUsedInMB = bytesUsed/(1024 * 1024);
 
    Serial.print("Bytes used: ");
    Serial.println(bytesUsedInMB);
    
/*  // initialisation de la carte micro SD
  // en mode 1 bit: plus lent, mais libère des broches
  if (SD_MMC.begin("/sdcard",true))
  { 
    uint8_t cardType = SD_MMC.cardType();
    if (cardType != CARD_NONE)
    {
      Serial.println("Carte SD Initialisee.");
      cartePresente = true;
    }
  }
*/
}
 
void loop()
{
}
