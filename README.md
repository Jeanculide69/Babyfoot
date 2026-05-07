# ⚽ Babyfoot Force - Star Wars V1.0 🌌

Bienvenue dans le dépôt du contrôleur de Babyfoot Star Wars. Ce projet transforme un babyfoot classique en une console de tournoi immersive avec effets visuels et sonores basés sur l'univers de Star Wars.

## 🚀 Fonctionnalités Principales
- **Affichage Matrix LED (64x32)** : HUD dynamique avec stickmen animés combattant au sabre laser.
- **Thème 2 "Force"** : Animations GIF haute qualité optimisées pour ESP32.
- **Mode Tournoi Autonome** : Gestion de tournois de 4 à 8 équipes avec progression automatique des brackets.
- **WiFi Manager & Portail Captif** : Configuration facile du WiFi via une interface web mobile.
- **TV Live Dashboard** : Page web dédiée pour afficher les scores et le tableau du tournoi sur un grand écran.
- **Mises à jour OTA** : Possibilité de téléverser des fichiers `.bin` directement via l'interface web.
- **Ambilight** : Bandeaux LED NeoPixel synchronisés avec l'action.
- **Audio immersif** : Bruitages de sabre laser, ambiance de stade et voix via DFPlayer Mini.

## 🛠 Matériel Utilisé (Hardware)
- **Microcontrôleur** : ESP32 DevKit V1
- **Dalle LED** : HUB75 Matrix Panel 64x32 (P3 ou P4)
- **Audio** : Module DFPlayer Mini + Haut-parleur 3W
- **Éclairage** : Bandeaux LED WS2812B (NeoPixels) x2 (40 LEDs par côté)
- **Capteurs de But** : Capteurs IR (TCRT5000 ou barrière IR)
- **Boutons** : 3 Boutons (LESS, MORE, OK) pour le contrôle manuel

## 📌 Brochage (Pinout ESP32)

| Fonction | Pin (GPIO) | Description |
| :--- | :--- | :--- |
| **Matrix R1, G1, B1** | 15, 2, 0 | Données couleur ligne 1 |
| **Matrix R2, G2, B2** | 4, 16, 17 | Données couleur ligne 2 |
| **Matrix A, B, C, D** | 5, 18, 19, 21 | Adressage des lignes |
| **Matrix CLK, LAT, OE** | 22, 3, 23 | Horloge, Verrou, Sortie |
| **DFPlayer TX / RX** | 12 / 13 | Communication Série Audio |
| **NeoPixel Strip 1 / 2** | 26 / 14 | Bandeaux Ambilight |
| **Capteur But Gauche / Droit** | 34 / 36 | Détection entrée balle |
| **Capteur Gamelle G / D** | 35 / 39 | Détection "gamelle" |
| **Boutons OK / LESS / MORE**| 27 / 32 / 33 | Contrôle manuel |
| **Reset Hardware Capteurs** | 25 | Réinitialisation des capteurs |

## 📚 Logiciels et Bibliothèques
- **Framework** : Arduino ESP32 Core **v2.0.2**
- **Libraries** :
  - `ESP32-HUB75-MatrixPanel-I2S-DMA` (Gestion Matrix)
  - `Adafruit_NeoPixel` (Gestion LEDs Ambilight)
  - `ArduinoJson` (v6.x - Gestion des tournois)
  - `WiFi`, `WebServer`, `LittleFS`, `DNSServer`, `HTTPUpdateServer` (Core ESP32)

## ⚙️ Installation & Compilation
1. **Partitionnement** : Dans l'IDE Arduino, sélectionnez `Partition Scheme: "Minimal SPIFFS (1.9MB APP with OTA)"`.
2. **Système de fichiers** : Téléversez le contenu du dossier `data/` (si présent) via l'outil LittleFS Data Upload.
3. **Identifiants WiFi** : Par défaut, le système tente de se connecter à la Livebox configurée. En cas d'échec, connectez-vous au réseau `Babyfoot-Force-StarWars` (IP: `192.168.4.1`) pour configurer votre propre réseau.

## 📜 Crédits & Version
- **Version** : 1.0 (Star Wars Edition)
- **Statut** : Stable
- **Date** : 08/05/2026

---
*Que la Force soit avec votre Babyfoot !*
