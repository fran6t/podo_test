# Prototype de capteur de mouvement pour semelle de chaussure

[![Licence MIT](https://img.shields.io/badge/Licence-MIT-green.svg)](#licence)

## Sommaire

* [Objectif](#-objectif)
* [Architecture système](#-architecture-système)
* [Fonctionnalités principales](#-fonctionnalités-principales)
* [Liste des composants](#-liste-des-composants-phase-1--prototype)
* [Optimisation du stockage](#-optimisation-du-stockage)
* [Gestion de l’alimentation](#-gestion-de-lalimentation)
* [Identification & transfert de données](#-identification--transfert-de-données)
* [Outils recommandés](#-outils-recommandés)
* [Licence](#licence)

## 🎯 Objectif

Concevoir un dispositif **compact et autonome** capable d’enregistrer l’activité (nombre de pas) lorsqu’il est logé dans la semelle d’une chaussure.

**Contraintes cibles :**

* **Acquisition** : 4 h par jour
* **Autonomie**   : 14 jours consécutifs

Le prototype utilise la carte **Waveshare ESP32‑S3‑Matrix** (25 × 25 mm) intégrant :

* ESP32‑S3 (Wi‑Fi/BLE)
* 2 Mo de PSRAM / 4 Mo de Flash
* Port USB‑C (pour debug uniquement)
* Matrice LED RGB 8 × 8 (non utilisé et trop consommateur)
* Capteur QMI8658 (IMU 6 axes : 3 gyr + 3 acc)

---

## 🖥️ Architecture système

```text
+----------------------+        +--------------+
|  ESP32‑S3‑Matrix     | I²C    |   QMI8658    |
| (25 × 25 mm)         |<------>| VCC 3.3 V    |
|                      |        +--------------+
|    GPIO9  (SDA)------+
|    GPIO8  (SCL)------+
| 5 V USB‑C → TP4056 → 🔋 Li‑Po 3.7 V ≥ 500 mAh |
+----------------------+        Autonomie ≥ 56 h
```

### Interfaces

| Bus / périphérique | Broches ESP32‑S3         | Débit      |
| ------------------ | ------------------------ | ---------- |
| **I²C** – QMI8658  | GPIO8 (SCL), GPIO9 (SDA) | 400 kHz    |
| **UART0** – Debug  | USB‑C (CDC)              | 115 200 Bd |

---

## 🌟 Fonctionnalités principales

* **Identification unique** de l’appareil (MAC Wi‑Fi + UID en flash)
* **Comptage de pas matériel** via l’algorithme embarqué du QMI8658
* **Réveil par interruption** (mouvement) → sortie de deep‑sleep
* **Stockage local** des pas (format binaire compact) sur MicroSD
* **Sync Wi‑Fi** : upload vers serveur LAMP à la demande

---

## 📦 Liste des composants (Phase 1 – Prototype)

| Réf.                             | Qté | Description                            |
| -------------------------------- | --: | -------------------------------------- |
| Waveshare ESP32‑S3‑Matrix        |   1 | Module 25 × 25 mm, Wi‑Fi/BLE + MicroSD |
| TP4056 + protection Li‑Po        |   1 | Chargeur USB‑C 1S + BMS                |
| Batterie Li‑Po 3.7 V ≥ 500 mAh   |   1 | Autonomie > 56 h                       |
| Connecteurs / fils silicones     |   — | Câblage interne flexible               |
| Boîtier PLA/PETG (impression 3D) |   — | Intégration dans la semelle            |

---

## 🚀 Optimisation du stockage

1. **Format binaire** 16 bits signé → \~3 × plus compact qu’un CSV.
2. **Écriture par blocs** (60 s) → moins de flush, débit SD maximal.
3. **Compression optionnelle** RLE sur séquences nulles (économie ≈ 12 %).

---

## 🔋 Gestion de l’alimentation

| État                   | I (typ.) | Durée / j |    Consommation |
| ---------------------- | -------: | --------: | --------------: |
| Acquisition active     |    25 mA |       4 h |         100 mAh |
| Deep‑Sleep (EXT0 wake) |    10 µA |      20 h |         0.2 mAh |
| **Budget / jour**      |          |           |   **≈ 100 mAh** |
| **Budget / 14 jours**  |          |           | **≈ 1 400 mAh** |

> Une batterie 800 mAh suffit si la charge est effectuée chaque semaine.

---

## 📡 Identification & transfert de données

### Workflow

1. **Boot** → activation du Wi‑Fi (AP + STA).
2. Requête HTTP `/hello` vers le serveur avec l’adresse MAC.
3. Le serveur renvoie une **commande** :

   * `START yyyy‑mm‑ddThh:mm:ssZ` → début d’acquisition
   * `DUMP` → envoi des enregistrements sous forme binaire
4. Données reçues ➜ **MySQL** → API REST → **front‑end JS/Chart.js**.

---

## 🛠️ Outils recommandés

* **IDE** : VS Code + PlatformIO (`platform = espressif32 @ 6.5.0`)
* **Alternative** : Arduino IDE 2.x
* **Data‑viz** : JavaScript + Chart.js
* **Modélisation 3D** : FreeCAD

---

## 🗺️ Roadmap

* [x] Prototype électronique + firmware base
* [ ] Calibration & validation du comptage de pas
* [ ] Design boîtier final & tests en conditions réelles
* [ ] Documentation (schémas, photos, tuto d’assemblage)
* [ ] Release v1.0 + dépôt GitHub public

---

## 📄 Licence

Ce projet est sous licence **MIT** – voir le fichier [`LICENSE`](LICENSE).

---
