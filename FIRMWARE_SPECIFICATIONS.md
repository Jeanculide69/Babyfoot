# 🛡️ SPÉCIFICATIONS OBLIGATOIRES DU FIRMWARE BABYFOOT

Ce document définit les contraintes matérielles et logicielles critiques qui **ne doivent jamais être modifiées** par un agent sans instruction explicite de l'utilisateur. Toute décision de conception doit impérativement respecter ces règles pour éviter de casser le matériel ou l'affichage.

## 1. 📺 Système d'Affichage (Matrice LED)
*   **Driver Obligatoire** : `ESP32-HUB75-MatrixPanel-I2S-DMA`.
    *   *Raison* : C'est le seul driver capable de fournir une image 100% stable sans scintillement sur cette architecture. Ne JAMAIS repasser sur une librairie de type "Bit-Banging" (ex: `ESP32RGBmatrixPanel`).
*   **Câblage HUB75 (NE PAS CHANGER)** :
    *   R1: 15 | G1: 2  | B1: 0
    *   R2: 4  | G2: 16 | B2: 17
    *   A: 5   | B: 18  | C: 19 | D: 21 | E: -1
    *   LAT: 3 | OE: 23 | CLK: 22
*   **Format de couleur** : Adafruit GFX standard (RGB565).

## 2. 🖐️ Entrées Utilisateur (Boutons Tactiles)
*   **Méthode** : `touchRead()`.
*   **Pins Dédiées** :
    *   Bouton OK : **27**
    *   Bouton MOINS (-) : **32**
    *   Bouton PLUS (+) : **33**
*   **Seuil de détection** : Fixé à **500** (Repos ~960, Touché ~200). Ne pas descendre sous 400 sans test physique.

## 3. ⚽ Capteurs de Buts (Digital)
*   **Logique** : Actif HAUT (`HIGH` quand déclenché).
*   **Pins Dédiées** :
    *   But Droit : **36**
    *   Gamelle Droite : **39**
    *   But Gauche : **34**
    *   Gamelle Gauche : **35**
*   **Contrainte ESP32** : Ces pins sont "Input-only". Ne JAMAIS utiliser `INPUT_PULLUP` sur ces pins (erreur GPIO garantie). Utiliser `INPUT` simple.

## 4. ⚙️ Logique Système
*   **Core ESP32** : Compatible avec la version 3.0+ (Utiliser la nouvelle API Timer si des timers matériels sont ajoutés).
*   **États du Jeu** : Utiliser les masques de bits définis dans `config.h` pour `statut_game` afin de rester compatible avec le reste du code historique.

## 5. 🔊 Audio (DFPlayer)
*   **Pins** : RX: 12, TX: 13.

---
*Dernière mise à jour : 06/05/2026 - Version Stable DMA V9.0*
