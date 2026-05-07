# ⚽ BABYFOOT STAR WARS - MASTER CONSOLE V1.0 🌌

## 🚨 RÈGLES D'OR (NE PAS MODIFIER SANS CONSENTEMENT)
- **HUD LED MATRIX** : L'affichage, le centrage des scores et le HUD Star Wars sont finalisés. NE PAS TOUCHER à la logique de dessin ou aux coordonnées.
- **INTERFACE WEB** : Le design du dashboard, de la TV et de l'organigramme de tournoi est validé. AUCUNE modification esthétique ou structurelle autorisée.
- **MAPPAGE DES PINS (GPIO)** : Les pins définies dans `config.h` sont synchronisées avec le hardware réel. NE PAS MODIFIER sous peine de destruction matérielle ou crash DMA.

## 🛠 SPÉCIFICATIONS TECHNIQUES
- **Plateforme** : ESP32 DevKit V1
- **Framework** : Arduino ESP32 Core **v2.0.2** (Stable)
- **Système de fichiers** : LittleFS (Partition 4MB)
- **Audio** : DFPlayer Mini (Serial1: 13/12)

## 📚 BIBLIOTHÈQUES UTILISÉES
- **ESP32-HUB75-MatrixPanel-I2S-DMA** : Gestion de la dalle LED en DMA haute fréquence.
- **Adafruit_NeoPixel** : Contrôle du bandeau Ambilight.
- **ArduinoJson** : Traitement des données de tournoi (JSON).
- **WiFi / WebServer / LittleFS** : Infrastructure réseau et stockage.

## 📡 RÉSEAU DE SECOURS
- **SSID AP** : Babyfoot-Force-StarWars
- **Mot de passe** : Aucun (Ouvert)
- **IP AP** : 192.168.4.1

---
*Dernière mise à jour : 06/05/2026 - Statut : STABLE V1.0*
