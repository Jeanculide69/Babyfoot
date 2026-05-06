# 🛡️ SPÉCIFICATIONS OBLIGATOIRES DU FIRMWARE BABYFOOT

Ce document définit les contraintes matérielles et logicielles critiques qui **ne doivent jamais être modifiées** sans instruction explicite. Toute décision de conception doit respecter ces règles pour garantir la stabilité du système (DMA) et l'intégrité de l'affichage.

## 1. 📺 Système d'Affichage (Matrice LED)
*   **Driver Obligatoire** : `ESP32-HUB75-MatrixPanel-I2S-DMA`.
*   **Configuration de base** : 64x32 pixels, 1 panel.
*   **Rotation (CRITIQUE)** : `matrix->setRotation(2)` (180°). Sans cela, l'affichage est inversé physiquement.
*   **Câblage HUB75** :
    *   R1: 15 | G1: 2  | B1: 0
    *   R2: 4  | G2: 16 | B2: 17
    *   A: 5   | B: 18  | C: 19 | D: 21 | E: -1
    *   LAT: 3 | OE: 23 | CLK: 22

## 2. 🎨 Interface Utilisateur (Star Wars HUD)
Le design actuel repose sur un thème "Jedi vs Sith".
*   **Layout des Scores (y=10)** :
    *   Score J1 (Bleu) : Gauche (x=1 ou x=5 si <10).
    *   Score J2 (Rouge) : Droite (x=42 ou x=51 si <10).
    *   Compteur de balles : Central (x=26, y=10).
*   **Zone d'Animation de Combat (Bas de l'écran)** :
    *   Zone réservée : **y=26 à y=31**.
    *   Nettoyage : Utiliser `matrix->fillRect(0, 26, 64, 6, C_BLACK)` à chaque frame d'animation pour ne pas effacer les scores.
*   **Format des Images/Animations** :
    *   Données stockées en format **444** (12-bit).
    *   Conversion obligatoire en **565** (RGB) avant l'affichage via le driver DMA.
*   **Logique d'Animation des Jedis (Stabilité)** :
    *   Les combattants doivent utiliser un système de `targetX` (cible aléatoire) pour éviter d'être statiques.
    *   Amplitude : Ils doivent pouvoir couvrir tout le bas de l'écran (`x=2` à `x=61`).
    *   Orientation : Toujours faire face à l'adversaire (dir calculé dynamiquement selon la position relative).

## 3. 🖐️ Entrées & Capteurs
*   **Boutons Tactiles (`touchRead`)** :
    *   OK: **27** | MOINS (-): **32** | PLUS (+): **33**
    *   Seuil de détection stable : **300**.
*   **Capteurs de Buts (Digital INPUT)** :
    *   But Droit: **36** | Gamelle Droite: **39**
    *   But Gauche: **34** | Gamelle Gauche: **35**
    *   **Logique** : Active-LOW (Détection sur passage à 0).
    *   *Note* : Pins "Input-only", Pull-up externes obligatoires (matériel).

## 4. ⚙️ Périphériques Additionnels
*   **Audio (DFPlayer Mini)** : RX: 12, TX: 13.
    *   **Méthode de lecture** : Commande `0x0F` (Spécifier Dossier/Fichier).
    *   **Organisation SD** : Dossier racine nommé `01`.
    *   **Contenu Dossier 01** :
        *   `001.mp3` : Intro Star Wars (Boucle)
        *   `002.mp3` : But Jedi (Short)
        *   `003.mp3` : But Sith (Short)
        *   `004.mp3` : Gamelle (Short)
        *   `005.mp3` : Victoire Jedi (Fanfare)
        *   `006.mp3` : Victoire Sith (Fanfare)
        *   `007.mp3` : Ambiance Match (Boucle Podrace)
        *   `008.mp3` : Signal Départ Match (Une fois avant le 007)
*   **LED Strips (NeoPixel)** : Pin **26** (Strip 1) et Pin **14** (Strip 2).
*   **Système** : Pin Reset hardware sur **25**.

---
*Dernière mise à jour : 06/05/2026 - Version Stable Audio V11.0 (Dossier 01)*
