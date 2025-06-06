void simulateSensorData() {
  if (!FILESYSTEM.begin(true)) {
    Serial.println("Erreur LittleFS");
    return;
  }

  File file = FILESYSTEM.open("/data.txt", FILE_WRITE);
  if (!file) {
    Serial.println("Erreur ouverture fichier");
    return;
  }

  int heure = 0;
  int minute = 0;
  int jour = 1;
  int mois = 1;
  int pas = 0;
  int totalnbJours = 0;
  const int nbJours = 90;

  const size_t TAILLE_LIGNE_ESTIMEE = 64;
  const size_t SEUIL_SECURITE = 256; // pour éviter de remplir à 100%

  while (totalnbJours <= nbJours) {
    bool actif = false;

    if (heure >= 7 && heure <= 22) {
      actif = (random(100) < 70);
    } else if (random(1000) < 3) {
      actif = true;
    }

    int previousPas = pas;

    if (actif) {
      pas += random(5, 15);
    }

    if (pas != previousPas) {
      // Vérifie espace libre
      size_t freeSpace = FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes();
      if (freeSpace < TAILLE_LIGNE_ESTIMEE + SEUIL_SECURITE) {
        file.println("[SIMULATION ARRETEE - MANQUE D'ESPACE]");
        Serial.println("❌ Espace insuffisant, simulation arrêtée.");
        break;
      }

      char ligne[64];
      snprintf(ligne, sizeof(ligne), "%d;%02dh%02d;%02d/%02d\n", pas, heure, minute, jour, mois);
      file.print(ligne);
    }

    // Avancer de 5 minutes
    minute += 5;
    if (minute >= 60) {
      minute = 0;
      heure++;
    }
    if (heure >= 24) {
      heure = 0;
      jour++;
      totalnbJours++;
      if (jour > 31) {
        jour = 1;
        mois++;
        if (mois > 12) mois = 1;
      }
    }
  }

  size_t total = FILESYSTEM.totalBytes();
  size_t used = FILESYSTEM.usedBytes();
  size_t freeBytes = total - used;

  file.printf("[Fin de simulation] Total utilisé : %lu / %lu bytes (libre : %lu)\n", used, total, freeBytes);
  file.close();

  Serial.printf("✅ Simulation terminée. Espace utilisé : %.2f KB / %.2f KB\n", used / 1024.0, total / 1024.0);
}
