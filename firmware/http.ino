String espName = "mac_inconnue"; // nom par défaut

void checkServerAndMaybeSend() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Adresse MAC comme identifiant unique
    String espID = WiFi.macAddress();
    espID.replace(":", "");

    // Construire l'URL avec l'ID
    String url = String(checkURL) + "?id=" + espID;
    http.begin(url);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println("Réponse reçue : " + payload);

      // Parsing JSON
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        espName = doc["nom"] | "mac_inconnue";
        Serial.println("📛 Nom reçu : " + espName);

        if (doc["Emettre"] == "oui") {
          Serial.println("⚠️ Emettre = oui → envoi du fichier");
          sendFile(espName);
        } else {
          Serial.println("ℹ️ Emettre = non, pas d'envoi");
        }

      } else {
        Serial.println("❌ Erreur parsing JSON");
      }

    } else {
      Serial.printf("❌ Erreur HTTP : %d\n", httpCode);
    }

    http.end();
  }
}


// Envoie le fichier SPIFFS via POST multipart
void sendFile(String espName) {
  File file = FILESYSTEM.open("/data.txt", FILE_READ);
  if (!file) {
    Serial.println("❌ Erreur : impossible d'ouvrir data.txt pour lecture");
    return;
  }

  // Obtenir la MAC (déjà sans ":")
  String mac = WiFi.macAddress();
  mac.replace(":", "");

  // Construire l'URL avec nom + MAC
  String url = String(uploadURL) + "?nom=" + espName + "&mac=" + mac;
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "text/plain");

  Serial.println("📤 Envoi du fichier à : " + url);

  int httpResponseCode = http.sendRequest("POST", &file, file.size());

  if (httpResponseCode > 0) {
    Serial.printf("✅ Fichier envoyé. Code HTTP : %d\n", httpResponseCode);
    String response = http.getString();
    Serial.println("📨 Réponse du serveur :");
    Serial.println(response);
  } else {
    Serial.printf("❌ Échec d'envoi. Erreur : %s\n", http.errorToString(httpResponseCode).c_str());
  }

  file.close();
  http.end();
}

