# Babyfoot Star Wars - Master Console V2.3 🚀

Ce projet est la version finale stabilisée et optimisée pour les tournois de Babyfoot.

## 📌 Spécifications Techniques

- **Microcontrôleur** : ESP32 DevKit V1
- **Affichage** : Matrice LED HUB75 (64x32) via I2S DMA
- **Audio** : DFPlayer Mini (Serial1: Pins 13/12)
- **Stockage** : LittleFS (Partition 1.5MB pour GIFs et JSON)
- **Capteurs** : Tactiles capacitifs pour les boutons, Infrarouges pour les buts.

## 🛠 Bibliothèques Requises (Versions Testées)

1. **ESP32-HUB75-MatrixPanel-I2S-DMA** (v3.0.x) : Pour la gestion de la matrice LED.
2. **Adafruit GFX** : Base graphique.
3. **AnimatedGIF** : Pour le décodage des animations Star Wars.
4. **ArduinoJson** (v6.x) : Pour la gestion du fichier `tournament.json`.
5. **WebSockets** (by Links2004) : Pour le dashboard TV en temps réel.
6. **Adafruit NeoPixel** : Pour les rubans LED latéraux.

## ⚙️ Réglages Critiques (NE PAS MODIFIER)

| Paramètre | Valeur | Raison |
| :--- | :--- | :--- |
| `SENS_SET` | **40** | Seuil de déclenchement tactile. Une valeur trop haute (ex: 400) provoque des déclenchements fantômes. |
| `DELAY_BUTTOM` | **5** | Filtrage anti-rebond. Calibré pour la vitesse de cycle de la V2.3. |
| `webTask Delay` | **10ms** | Temps de repos pour le stack WiFi. Un délai plus court sature le processeur et ralentit le portail. |
| `FS Partition` | **1.5MB** | Nécessaire pour stocker tous les fichiers GIF du thème Star Wars. |

## 🌐 Portail Web & Dashboard

- **Console** : `http://babyfoot.local/` (Gestion des scores et actions)
- **Stadium TV** : `http://babyfoot.local/tv` (Affichage spectateurs avec WebSockets sur port 81)
- **Optimisation** : Les réponses API sont générées par concaténation de chaînes pour une réactivité maximale (plus rapide que le JSON classique).

---
*Version 2.3 - Stable & Validée*