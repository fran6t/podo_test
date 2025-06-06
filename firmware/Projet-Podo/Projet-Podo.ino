#include <WiFi.h>
//#include <SPIFFS.h>
#include "LittleFS.h"
#define FILESYSTEM LittleFS
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Voir secrets_example.h pour faire votre fichier secrets.h
// C'est lui qui va contenir les url endpoint et les infos de connexion wifi 
#include "secrets.h"



// Timer
unsigned long lastCheck = 0;
const unsigned long interval = 15000; // 15 secondes

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connecté.");

  if (!LittleFS.begin(true)) {
    Serial.println("Erreur LittleFS");
    return;
  }


  // Simuler une lecture capteur et l’enregistrer
  simulateSensorData();
}

void loop() {
  if (millis() - lastCheck >= interval) {
    lastCheck = millis();
    checkServerAndMaybeSend();
  }
}