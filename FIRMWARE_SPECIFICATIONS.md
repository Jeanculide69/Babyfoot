# Babyfoot Star Wars ESP32 - Spécifications Firmware V11.4 (Stable)

## 🌌 Résumé du Système
Firmware optimisé pour ESP32 (Dual Core) gérant un écran LED Matrix HUB75 (64x32), un système audio DFPlayer Mini, et un ruban LED Ambilight.

## 🕹️ Règles du Jeu (Mise à jour)
- **Score Max** : Non plafonné, mais bloqué à **0 minimum** (pas de scores négatifs).
- **Gamelle** : Déclenche l'animation et retire 1 point à l'adversaire, mais **ne décompte pas de balle** du réservoir (la balle reste sur le terrain).
- **Victoire** : Se déclenche quand le réservoir de 11 balles est vide.

## 📱 Console de Commande WiFi
- **Accès** : Se connecte à la Livebox (`Livebox-6E60`). Accessible via `http://babyfoot.local`.
- **Fonctions** :
  - **Live Score** : Affichage en temps réel des scores et balles sur le téléphone.
  - **Éditeur de Score** : Boutons `+` et `-` pour corriger manuellement les scores.
  - **Simulateur** : Boutons But et Gamelle pour tester les animations.
  - **Luminosité** : Slider pour régler l'intensité de l'écran LED en direct.

## 🌈 Système Ambilight (LEDs)
- **Pins** : GPIO 26 (Ruban 1) et GPIO 14 (Ruban 2).
- **Mode Match** : Blanc froid intense pour éclairer le terrain.
- **Mode Animation** : Effet arc-en-ciel dynamique pendant les buts et victoires.
- **Mode Veille** : Pulse bleu "Force" discret.

## 🖥️ Matrice LED & Performance
- **Fréquence** : Bridée à **15 FPS** (60ms par frame) pour garantir la stabilité WiFi et Audio.
- **Transitions** : Nettoyage complet de l'écran (`fillScreen`) lors des changements d'états pour éviter le "ghosting".
- **HUD Fixe** : Les scores restent affichés par-dessus les animations.

## 🔊 Audio (DFPlayer)
- **Dossier 01/** :
  - `001.mp3` : Intro
  - `002.mp3` : But J1
  - `003.mp3` : But J2
  - `004.mp3` : Gamelle
  - `005.mp3` : Victoire J1
  - `006.mp3` : Victoire J2
  - `007.mp3` : Ambiance Match (Boucle)
  - `008.mp3` : Signal Départ (Handshake)

## 🛠️ Pinout Hardware
| Fonction | Pin ESP32 |
| :--- | :--- |
| **Bouton OK (Tactile)** | 27 |
| **Bouton + / -** | 33 / 32 |
| **Capteurs Buts L / R** | 34 / 36 |
| **Capteurs Gamelles L / R** | 35 / 39 |
| **Reset Hardware Capteurs** | 25 |
| **DFPlayer TX / RX** | 13 / 12 |
| **NeoPixel 1 / 2** | 26 / 14 |
| **Matrix LAT / OE / CLK** | 3 / 23 / 22 |
