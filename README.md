# Babyfoot Master Console - Star Wars Edition
## Version 2.5 FINAL (11/05/2026)

Ce projet est le firmware d'une console de gestion de Babyfoot basée sur un ESP32, avec une interface visuelle Star Wars sur panneau LED 64x32 et une intégration tournoi automatisée.

### Architecture du Code

- **babyfoot_esp32.ino** : Point d'entrée principal. Gère les tâches multi-coeurs (Core 0 pour le Web, Core 1 pour le Jeu), les variables globales de score et les routes du serveur HTTP.
- **game_logic.ino** : Cœur de la machine à états du jeu. Gère la détection des buts (capteurs optiques), les boutons physiques, et la logique de progression des tournois.
- **read_gif_file.ino** : Gestionnaire d'animations GIF utilisant LittleFS et la librairie AnimatedGIF. Gère aussi l'effet Ambilight (bandeaux LED) synchronisé avec les animations.
- **config.h** : Configuration matérielle (pins, résolutions, seuils de sensibilité) et constantes logiques.

### Fonctionnalités Clés

1. **Tournoi Automatisé** : Le système lit un fichier `tournament.json` et enchaîne les matchs. Après chaque match, une "Pause Bière" est lancée, puis le système se met en attente (écran VS clignotant) jusqu'à ce que les joueurs appuient sur OK.
2. **Effets Spéciaux** : Animations GIF thématiques (Buts, Gamelles, Demi, Victoire) synchronisées avec des effets sonores (SFX) via un module audio externe.
3. **Ambilight Dynamique** : Bandeaux LED NeoPixel qui pulsent ou changent de couleur selon l'état du jeu et l'animation en cours.
4. **Dashboard TV** : Interface web déportée (/TV) pour afficher les scores et l'ambiance du match en temps réel.

### Installation & Maintenance

- **Sensibilité** : Réglée à 400 avec un filtrage d'une seconde pour éviter les rebonds.
- **LittleFS** : Toutes les animations (.gif) et les sons doivent être présents dans le système de fichiers de l'ESP32.
- **Loop Standby** : L'animation `veille.gif` tourne en boucle fluide sans freeze entre les répétitions.

---
*Développé avec Antigravity AI pour une expérience Babyfoot ultime.*