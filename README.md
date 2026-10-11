[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/clan-2Ft)

# Projet de programmation — Jeu d'exploration en monde infini

## Membres du groupe

| Matricule |
|----------------|
| 609539         |
|        615667        |
|597738                |
|606644      |

## Instructions de lancement
l'exécutable se trouve dans le dossier : info-h2001-projet-ba2-v-e-r-g -> build -> bin -> main.exe

## Description du projet

**Element Island** est un jeu d'exploration isométrique inspiré de l'univers d'Avatar : Le Dernier Maître de l'Air. Le joueur incarne un avatar capable de maîtriser les quatre éléments — Feu, Eau, Air et Terre — et doit explorer un monde composé d'îles thématiques générées procéduralement.

L'objectif est de compléter les quatre quêtes élémentales : parler aux gardiens de chaque île, accomplir leur défi, et récupérer les reliques. Une fois les quatre reliques obtenues, le joueur remporte la victoire. Des ennemis apparaissent régulièrement et deviennent de plus en plus puissants avec le temps.

## Deadline

Le projet doit être soumis pour le 15 Mai à 23h59. Toute soumission après cette date ne sera pas prise en compte.

Lisez bien les instructions de l'énoncé du projet à ce sujet.

## Fonctionnalités implémentées

### Contraintes obligatoires

- [x] Un projet GitHub
- [x] Hiérarchie de classes (héritage, polymorphisme) — `Entity` est la classe de base de `Player`, `Enemy` et `Neutral` ; toutes les entités sont stockées dans un `vector<unique_ptr<Entity>>` et gérées via polymorphisme
- [x] Affichage 2D — interface graphique isométrique avec SFML 3.0
- [x] Monde infini — système de chunks de 32×32 tuiles chargés et déchargés dynamiquement autour du joueur
- [x] Génération procédurale du terrain et des entités — deux bruits de Voronoï (`shapeNoise` et `biomeNoise`) via FastNoiseLite pour les îles et les biomes ; placement déterministe des reliques par hash
- [x] Persistance en mémoire — les modifications de tiles (ponts créés, volcans éteints) sont conservées dans une liste de `TileChange` et réappliquées au rechargement des chunks
- [x] Sauvegarde et chargement — sauvegarde JSON (nlohmann/json) de la position, HP, kills, pouvoirs, quêtes et bateau via F5 ou à la fermeture
- [x] Éléments infranchissables — l'eau (tile 0) est infranchissable à pied
- [x] Éléments interactifs ou destructibles — volcans éteignables avec le pouvoir Eau ; fragments collectables en bateau ; reliques ramassables en marchant dessus
- [x] Entité autonome ou semi-autonome — les ennemis poursuivent le joueur de façon autonome ; les Neutrals se déplacent aléatoirement sur leur île
- [x] Mécanisme de progression — 4 pouvoirs élémentaux améliorables (firePower, waterPower, airPower, earthPower) en ramassant des reliques ; difficulté croissante via `enemyPower` ; système de 4 quêtes

### Bonus

- [x] Interface graphique (bibliothèque : SFML 3.0)
- [x] Sprites animés directionnels pour le joueur (16 lignes × 6 frames), les ennemis et les Neutrals (Fisherman spritesheet)
- [x] Système de quêtes avec dialogues et objectifs distincts par île
- [x] Bateau navigable avec carte au trésor pour la quête de l'île de Terre
- [x] Boussole dynamique pointant vers les Neutrals non encore visités
- [x] Écran de titre animé avec les 4 éléments dessinés en code
- [x] Musique de fond en boucle (island_0.ogg)
- [x] Système de meilleurs scores sauvegardé entre les parties (kills + temps de complétion des quêtes)
- [x] Animation de complétion de quête (anneaux colorés)

## Compilation et exécution

### Prérequis

- CMake 3.28 ou supérieur
- Compilateur C++17 (g++, clang++, MSVC)
- Connexion Internet pour le premier build (SFML et nlohmann/json téléchargés automatiquement via FetchContent)

### Instructions

```bash
cmake -B build          # Génère les fichiers de build
cmake --build build     # Compile le projet
./build/bin/main        # Lance le jeu
```

L'exécutable se trouve dans `build/bin/main`.

## Contrôles

| Touche | Action |
|--------|--------|
| Z / Q / S / D | Déplacer le joueur |
| 1 | Sélectionner l'élément Feu |
| 2 | Sélectionner l'élément Eau |
| 3 | Sélectionner l'élément Air |
| 4 | Sélectionner l'élément Terre |
| Espace | Utiliser le pouvoir élémental |
| E | Parler au Neutral le plus proche |
| X | Fermer la boîte de dialogue |
| B | Embarquer / Débarquer du bateau |
| F5 | Sauvegarder la partie |
| Échap | Quitter |
| R | Rejouer (écran Game Over / Victoire) |

## Architecture du projet

```
src/
├── main.cpp              — Boucle principale, gestion des événements, rendu
├── Entity.h/cpp          — Classe de base abstraite (position, HP, draw, update)
├── Player.h/cpp          — Joueur : déplacement, pouvoirs, HUD, animation
├── Enemy.h/cpp           — Ennemi : poursuite autonome, barres de vie, sprites
├── Neutral.h/cpp         — PNJ gardien : statique sur son île, dialogue
├── World.h/cpp           — Gestion des chunks, sauvegarde, collisions
├── Chunk.h/cpp           — Génération procédurale d'un chunk, maillage isométrique
├── Tile.h/cpp            — Tuile individuelle (id)
├── TileRegistry.h/cpp    — Registre des types de tuiles (nom, walkable, interactable)
├── TileChange.h          — Structure pour stocker les modifications de tiles
├── LoadedChunk.h         — Structure associant un chunk à ses coordonnées relatives
├── NoiseGen.h/cpp        — Encapsulation de FastNoiseLite (getShape + getBiome)
├── FastNoiseLite.h       — Bibliothèque de génération de bruit
├── Island.h/cpp          — Structure d'île (non finalisée)
├── QuestManager.h/cpp    — Système de quêtes, dialogues, HUD des reliques
├── Boat.h/cpp            — Navigation, carte au trésor, collecte de fragments
├── RelicRender.h/cpp     — Rendu des reliques sur les tiles
├── TileFinder.h/cpp      — Recherche de tiles par type dans les chunks chargés
├── Compass.h/cpp         — Boussole pointant vers les Neutrals
├── ScoreSave.h/cpp       — Chargement et sauvegarde des meilleurs scores (JSON)
├── TitleScreen.h/cpp     — Écran de titre animé
└── GameOver.h/cpp        — Écran de Game Over avec meilleurs scores
assets/
├── textures/             — Spritesheets (joueur, ennemis, neutral, bateau, tiles)
├── fonts/                — Police arial.ttf
└── island_0.ogg          — Musique de fond
savefiles/
├── savefile.json         — Sauvegarde de la partie en cours
└── highscore.json        — Meilleur score (kills + temps)
```

## Choix techniques

**Génération procédurale** : On utilise deux bruits de Voronoï via FastNoiseLite — `shapeNoise` pour savoir si une position est dans l'océan, sur la plage ou sur une île, et `biomeNoise` pour choisir le type d'île (Feu, Eau, Air ou Terre). Les reliques sont placées avec un hash déterministe basé sur les coordonnées du chunk, ce qui fait que la carte est toujours la même à chaque lancement.

**Système de chunks** : Le monde est découpé en chunks de 32×32 tuiles. On charge uniquement les chunks visibles autour du joueur et on décharge les autres pour ne pas surcharger la mémoire. Les modifications faites en jeu (volcans éteints, ponts créés) sont sauvegardées dans une liste de `TileChange` et réappliquées automatiquement quand un chunk est rechargé.

**Polymorphisme** : `Player`, `Enemy` et `Neutral` héritent tous de la classe de base `Entity`. On les stocke ensemble dans un `vector<unique_ptr<Entity>>` et on utilise `dynamic_cast` pour accéder aux comportements spécifiques de chaque type. Ça nous a permis de gérer toutes les entités avec une seule boucle.

**Sauvegarde** : On sauvegarde tout en JSON avec nlohmann/json — la position et les stats du joueur, l'état des quêtes, la position du bateau et ses fragments, et toutes les modifications de tiles. Ça nous semblait plus lisible et plus facile à déboguer qu'un format binaire.

**Rendu isométrique** : Chaque chunk construit un `sf::VertexArray` de triangles directement depuis la spritesheet. Les positions isométriques sont calculées avec `gridStepX = 26.0f` et `gridStepY = 13.0f`, qui correspondent à la moitié et au quart de la taille d'une tuile (52×52px).
