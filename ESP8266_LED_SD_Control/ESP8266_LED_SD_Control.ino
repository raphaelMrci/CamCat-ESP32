/*
  This is a demo of ESP8266 WiFi Module in standalone mode (without Arduino) controlling an LED via a Web page.
  This code also demostrates that PWM is available on the ESP8266, so we can dim the LED.
  Analog Input is available on the ESP8266 but this pin is not wired up on ESP-01 board.
  The ESP can also support interrupts.

  This demo uses the Arduino IDE and the Arduino Boards Manager to upload the program to the ESP2866 with an FTDI Programmer.
  N.B. This overwrites any Firmware that was previously on the ESP8266.

  See: http://www.instructables.com/id/ESP8266-WiFi-Module-for-Dummies/ for the details on how to set the board up and upload the code to the ESP8266
  Also see: http://www.instructables.com/id/The-Simple-Guide-to-Flashing-Your-ESP8266-Firmware/
*/

// Include the ESP8266 Library. This library is automatically provided by the ESP8266 Board Manager and does not need to be installed manually.
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>

String codeVersion = "Version 1.0  Aug 2016 by TonesB";

// WiFi Router Login - change these to your router settings
//char* SSID = "No mind the dog, beware of owner";
//char* password = "20460701";

// Setup GPIO2
int pinGPIO2 = 16; //To control LED
int ledStatus = 0; //0=off,1=on,2=dimmed

// Create the ESP Web Server on port 80
WiFiServer WebServer(80);
// Web Client
WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println(codeVersion);

  // Setup the GPIO2 LED Pin - this demo also uses PWM to dim the LED.
  pinMode(pinGPIO2, OUTPUT);
  digitalWrite(pinGPIO2, LOW);
  ledStatus = 0;

  Serial.println();

  // Connect to WiFi network
  if (!SD.begin(4)) {
    Serial.println("Carte SD introuvable");
    return;
  }
  Serial.println("Carte SD installee");

  char* SSID=Lecture_fichier("/ssid2.txt");

  char* password=Lecture_fichier("/passe2.txt");

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  Serial.print("Connexion sur ");
  Serial.println(SSID);
  Serial.println(password);
  WiFi.begin(SSID, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connection sur Wifi realisee");

  // Start the Web Server
  WebServer.begin();
  Serial.println("Serveur WEB installe");

  // Print the IP address
  Serial.print("Vous pouvez vous connecter avec IP : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Check if a user has connected
  client = WebServer.available();
  if (!client) {//restart loop
    return;
  }

  // Wait until the user sends some data
  Serial.println("Nouvel utilisateur");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r\n');
  Serial.println(request);
  client.flush();

  // Process the request:
  if (request.indexOf("/LED=ON") != -1) {
    analogWrite(pinGPIO2, 0);
    ledStatus = 1;
  }
  if (request.indexOf("/LED=OFF") != -1) {
    analogWrite(pinGPIO2, 1023);
    ledStatus = 0;
  }
  if (request.indexOf("/LED=DIM") != -1) {
    analogWrite(pinGPIO2, 922);
    ledStatus = 2;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>ESP8266 Demo</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<a href=\"/\">Refresh Status</a>");
  client.println("</br></br>");

  //check the LED status
  if (ledStatus == 0) {
    client.print("LED is Off</br>");
    client.println("Turn the LED <a href=\"/LED=ON\">ON</a></br>");
    client.println("Set LED to <a href=\"/LED=DIM\">DIM</a></br>");
  } else if (ledStatus == 1) {
    client.print("LED is On</br>");
    client.println("Turn the LED <a href=\"/LED=OFF\">OFF</a></br>");
    client.println("Set LED to <a href=\"/LED=DIM\">DIM</a></br>");
  } else if (ledStatus == 2) {
    client.print("LED is Dimmed</br>");
    client.println("Turn the LED <a href=\"/LED=OFF\">OFF</a></br>");
    client.println("Turn the LED <a href=\"/LED=ON\">ON</a></br>");
  }

  client.println("</br>");
  client.println("<a href=\"http://www.instructables.com/id/ESP8266-WiFi-Module-for-Dummies/\" target=\"_blank\">See the Instructables Page: ESP8266 WiFi Module for Dummies</a></br>");
  client.println("<a href=\"http://www.instructables.com/id/The-Simple-Guide-to-Flashing-Your-ESP8266-Firmware/\" target=\"_blank\">See the Instructables Page: The Simple Guide to Flashing Your ESP8266 Firmware</a></br>");

  client.println("</br>");
  client.println("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("Utilisateur deconnecte");
  Serial.println("");

}
