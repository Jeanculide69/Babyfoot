# ⚽ Star Wars Babyfoot : Master Console V3.0 FINAL 🚀

![Version](https://img.shields.io/badge/Version-3.0_Final-gold?style=for-the-badge&logo=starwars)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif)
![Status](https://img.shields.io/badge/Status-Stable%20&%20Deploiement-success?style=for-the-badge)

Bienvenue dans le centre de commande ultime pour votre Babyfoot. Cette console transforme un Babyfoot classique en une arène intergalactique avec gestion de tournois automatisée, effets visuels Star Wars et immersion sonore totale.

---

## 🌟 Fonctionnalités Élite

### 🏆 Gestion de Tournoi Autonome
*   **Bracket Evolution** : Enchaînement automatique des matchs via `tournament.json`.
*   **Pause Bière Cinématique** : Transition de 15s après chaque match avec animation dédiée.
*   **Match Start Interactive** : Système "Wait for OK" pour afficher l'écran **VS** et ne lancer la musique de combat que sur validation des joueurs.

### 🎥 Expérience Visuelle & Sonore
*   **LED Matrix HUD** : Affichage 64x32 avec animations GIF fluides (Buts, Gamelles, Victoire).
*   **Ambilight Spatialisé** : Bandeaux NeoPixel synchronisés avec les actions (Pulsations, Stroboscope, Respiration).
*   **Audio Synchro** : Intégration d'un module sonore pour les musiques d'ambiance et SFX Star Wars.

### 🌐 Écosystème Connecté
*   **Dashboard TV** : Interface déportée ultra-réactive pour le public.
*   **Portail de Gestion** : Configuration live des scores, noms des équipes et gestion des fichiers LittleFS.

---

## 🛠️ Architecture du Projet

Le firmware est structuré pour maximiser la performance du dual-core de l'ESP32 :

| Fichier | Rôle | ✨ Core Features |
| :--- | :--- | :--- |
| `babyfoot_esp32.ino` | **Command Center** | Multi-tasking, Web Server, État global. |
| `game_logic.ino` | **Jedi Master** | Détection IR, Machine à états, Progression Tournoi. |
| `read_gif_file.ino` | **Hologram Engine** | Moteur GIF LittleFS, Ambilight dynamique, Boucle fluide. |
| `neopixel_logic.ino` | **Light Speed** | Gestion bas niveau des bandeaux de LEDs. |
| `config.h` | **The Source** | Sensibilité (400), Pins, Délais et Versioning. |
| `web_pages.h` | **Holonet** | Code HTML/JS des interfaces TV et Administrateur. |

---

## 🔧 Spécifications Techniques

### ⚙️ Paramétrage Capteurs
*   **Sensibilité** : `400` (Seuil de détection IR optimisé).
*   **Anti-rebond** : `1000ms` (Évite les scores multiples sur un seul but).
*   **Vitesse** : Dual-Core Tasking (Web sur Core 0 / Game Logic sur Core 1).

### 📂 Système de Fichiers (LittleFS)
*   `/Animations/` : Stockage des fichiers `.gif` (64x32 pixels).
*   `/tournament.json` : Stockage de la structure dynamique du tournoi.

---

## 🚀 Installation Rapide

1.  **Hardware** : ESP32 + Panneau LED Matrix I2S + Bandeaux NeoPixel.
2.  **Flash** : Compiler via Arduino IDE avec le support **LittleFS**.
3.  **Assets** : Uploader le contenu du dossier `Animations` dans la mémoire flash.
4.  **Play** : Allumez, validez le premier match, et que la Force soit avec vous !

---

## 📝 Journal des Modifications (V3.0 FINAL)
*   ✅ **LED Color Fix** : Correction de l'ordre des couleurs (Bleu/Vert) pour compatibilité hardware.
*   ✅ **Timing Optimization** : Transition jaune accélérée sur les moments critiques du jeu.
*   ✅ **Security** : Suppression totale des identifiants personnels du code source.

## 📝 Journal des Modifications (V2.6)

## 📝 Journal des Modifications (V2.5 FINAL)

*   ✅ **Seamless Loop** : Suppression du freeze d'une seconde sur l'animation de veille.
*   ✅ **Match Flow** : Nouvelle étape de confirmation OK obligatoire entre les matchs.
*   ✅ **Extended Anims** : Augmentation du buffer de temps pour les animations longues (Demi-finale/Pinte).
*   ✅ **Documentation** : Nettoyage complet du dépôt et ajout de ce README Premium.

---
*Projet réalisé avec passion pour l'expérience Babyfoot ultime.*
*"Do or do not, there is no try."*