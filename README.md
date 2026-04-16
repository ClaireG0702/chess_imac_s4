# Jeu d'Échecs C++

Un jeu d'échecs avec deux modes de jeu :

- **Mode Classique** : Échecs traditionnels
- **Mode Chaotique** : Échecs augmentés avec 8 événements aléatoires générés par des lois de probabilité

## Table des matières

- [Vue d'ensemble](#vue-densemble)
- [Fonctionnalités](#fonctionnalités)
- [Mode de jeu classique](#mode-de-jeu-classique)
- [Mode chaotique](#mode-chaotique)
- [Architecture](#architecture)
- [Installation et compilation](#installation-et-compilation)
- [Guide de jeu](#guide-de-jeu)
- [Contrôles](#contrôles)
- [Dépendances](#dépendances)
- [Licence](#licence)

---

## Vue d'ensemble

### Présentation générale

Ce projet est une implémentation complète d'un jeu d'échecs en **C++20** avec une interface graphique moderne utilisant **OpenGL 3.3** et **ImGui**. Le jeu propose une expérience immersive avec :

- **Rendu 3D** avec skybox et modèles GLTF
- **Interface intuitive** en temps réel
- **Validation des mouvements** légaux selon les règles d'échecs
- **Deux modes de jeu** pour différents styles de gameplay

### Contexte académique

Ce projet a été développé pour les cours **Programmation Algorithmique**, **Synthèse d'images** et **Mathématiques** à l'IMAC (Image Multimédia, Audiovisuel et Communication), promotion 2027.

---

## Fonctionnalités

### Fonctionnalités communes aux deux modes

**Implémentation complète du jeu d'échecs**

- Toutes les pièces : Pion, Cavalier, Fou, Tour, Reine, Roi
- Mouvements légaux validés pour chaque type de pièce
- Promotion des pions

**Interface utilisateur avancée**

- Sélection de pièces par clics souris
- Visualisation des coups possibles
- Menu de démarrage avec sélection de mode

**Rendu graphique immersif**

- Vue 3D du plateau d'échecs
- Contrôle de caméra interactive
- Skybox dynamique
- Modèles 3D des pièces en GLTF
- Éclairage Blinn-Phong

### Spécificités du Mode Classique

Jeu d'échecs traditionnel sans interruption

- Les règles classiques s'appliquent
- Jeu équilibré

### Spécificités du Mode Chaotique

**45% de chance de déclencher un événement chaotique par tour**

Chaque événement :

- Est **sélectionné aléatoirement** parmi les 8 possibles
- Utilise une **distribution mathématique spécifique** pour générer ses paramètres
- Transforme le gameplay de manière **contrôlée mais imprévisible**
- Bénéficie d'un **équilibrage basé sur les propriétés mathématiques**

---

## Mode de jeu classique

### Le mode classique implémente l'intégralité des règles d'échecs

## Mode chaotique

### 8 Événements Chaotiques

Les événements chaotiques utilisent les **distributions mathématiques** pour générer des situations de jeu imprévisibles.

### 1. **Révolution** — Des pions changent de camp

**Déclencheur** : 45% de chance par tour  
**Effet** : 2-3 pions aléatoires changent de camp et deviennent ennemis

| Aspect            | Détail                                         |
| ----------------- | ---------------------------------------------- |
| **Distributions** | Binomiale + Hypergéométrique                   |
| **Sélection**     | Tirage sans remise parmi les pions disponibles |

---

### 2. **Swap Reine ↔ Pion** — Transformations pion/reine

**Déclencheur** : 45% de chance par tour  
**Effet** : 0 à N pièces parmi les pions et reines échangent leur type

| Aspect            | Détail                                        |
| ----------------- | --------------------------------------------- |
| **Distributions** | Poisson + Uniforme Discrète                   |
| **Nombre**        | Variable (peut être 0, 1, 2+ transformations) |

---

### 3. **Cavalier Sauvage** — Une pièce devient cavalier

**Déclencheur** : 45% de chance par tour  
**Effet** : Une pièce aléatoire (sauf le roi) devient cavalier

| Aspect           | Détail                                        |
| ---------------- | --------------------------------------------- |
| **Distribution** | Exponentielle (sans mémoire)                  |
| **Sélection**    | Uniforme parmi toutes les pièces non-roi      |
| **Propriété**    | Peut se déclencher à tout moment sans pattern |

---

### 4. **Dyscalculie** — Erreur de mouvement

**Déclencheur** : 45% de chance par tour  
**Effet** : Un mouvement s'exécute avec ±N cases d'erreur

| Aspect           | Détail                                         |
| ---------------- | ---------------------------------------------- |
| **Distribution** | Cauchy (queues lourdes)                        |
| **Magnitude**    | Erreurs petites MAIS erreurs énormes possibles |

---

### 5. **Mutation** — Transformation de pièce aléatoire

**Déclencheur** : 45% de chance par tour  
**Effet** : Une pièce du joueur actif (sauf roi) change de type aléatoirement

| Aspect           | Détail                                |
| ---------------- | ------------------------------------- |
| **Distribution** | Chi-Carré (asymétrique)               |
| **Sélection**    | Parmi les pièces du joueur actif      |
| **Variabilité**  | Certains types peuvent être favorisés |

---

### 6. **Rupture d'Attention** — Faux mouvement

**Déclencheur** : 45% de chance par tour  
**Effet** : Votre pièce se déplace vers un coup légal DIFFÉRENT de celui choisi

| Aspect           | Détail                                |
| ---------------- | ------------------------------------- |
| **Distribution** | Uniforme Discrète                     |
| **Sélection**    | Parmi tous les coups légaux possibles |

---

### 7. **Fuite** — Coup isolé + fin de tour

**Déclencheur** : 45% de chance par tour  
**Effet** : Un mouvement s'exécute vers une case isolée ET votre tour s'arrête

| Aspect           | Détail                              |
| ---------------- | ----------------------------------- |
| **Distribution** | Exponentielle (rare + imprévisible) |

---

### 8. **Daltonisme** — Plateau gris 1 tour

**Déclencheur** : 45% de chance par tour  
**Effet** : Pendant 1 tour, tous les pions deviennent gris → jouez les deux couleurs

| Aspect           | Détail                                   |
| ---------------- | ---------------------------------------- |
| **Distribution** | Normale (Gauss)                          |
| **Durée**        | Exactement 1 tour (temporaire)           |
| **Liberté**      | Vous pouvez déplacer N'IMPORTE QUEL pion |

---

## Résumé des 8 Événements

| Événement   | Distribution      | Impact                    |
| ----------- | ----------------- | ------------------------- |
| Révolution  | Binomiale + Hyp.  | Changement de camp        |
| Swap        | Poisson + Unif.   | Transformation pion/reine |
| Cavalier    | Exponentielle     | Nouvelle menace           |
| Dyscalculie | Cauchy            | Mouvement erroné          |
| Mutation    | Chi-Carré         | Type aléatoire            |
| Rupture     | Uniforme Discrète | Faux coup                 |
| Fuite       | Exponentielle     | Coup isolé + fin          |
| Daltonisme  | Normale           | Plateau gris 1 tour       |

---

## Équilibrage Mathématique

### Probabilités par tour (Mode Chaotique)

- **45% de chance** qu'**un événement** se déclenche
- **1 événement sélectionné** aléatoirement parmi les 8 (12.5% chacun)
- **Paramètres variables** selon la distribution de l'événement

### Propriétés des distributions utilisées

**Distributions discrètes** (nombre entier)

- Binomiale : nombre d'occurrences fixe mais variable
- Poisson : rareté naturelle
- Uniforme Discrète : aucun biais

**Distributions continues** (fluides)

- Exponentielle : sans mémoire (memoryless) → imprévisible
- Cauchy : queues lourdes → extrêmes possibles
- Chi-Carré : asymétrique → biais intégré
- Normale : modulation douce autour d'une moyenne

---

## Architecture

### Structure du projet

```
chess_imac_s4/
├── src/                          # Code source principal
│   ├── main.cpp                  # Point d'entrée de l'application
│   ├── Game.cpp/hpp              # Boucle principale du jeu
│   │
│   ├── core/                     # Logique du jeu
│   │   ├── Board.cpp/hpp         # Gestion du plateau d'échecs
│   │   ├── GameState.cpp/hpp     # État du jeu (tours, pièces, historique)
│   │   ├── Piece.cpp/hpp         # Classe de base pour les pièces
│   │   ├── ChaoticEvent.cpp/hpp  # Manager des 8 événements chaotiques
│   │   └── pieces/               # Implémentations des pièces
│   │       ├── Pawn.cpp/hpp      # Pion
│   │       ├── Knight.cpp/hpp    # Cavalier
│   │       ├── Bishop.cpp/hpp    # Fou
│   │       ├── Rook.cpp/hpp      # Tour
│   │       ├── Queen.cpp/hpp     # Reine
│   │       └── King.cpp/hpp      # Roi
│   │
│   ├── ui/                       # Interface utilisateur et rendu
│   │   ├── Renderer.cpp/hpp      # Rendu ImGui du plateau 2D
│   │   ├── Renderer3D.cpp/hpp    # Rendu OpenGL 3D
│   │   ├── InputHandler.cpp/hpp  # Gestion des entrées souris/clavier
│   │   ├── GLTFLoader.cpp/hpp    # Chargement des modèles GLTF
│   │   ├── Mesh.cpp/hpp          # Gestion des mailles 3D
│   │   ├── PieceModel.cpp/hpp    # Modèles 3D des pièces
│   │   ├── Skybox.cpp/hpp        # Skybox 3D
│   │   ├── PieceAnimation.hpp    # Animations des pièces
│   │   ├── SelectionState.hpp    # État de sélection
│   │   └── GLHeaders.hpp         # Headers OpenGL/GLAD
│   │
│   ├── math/                     # Distributions de probabilité
│   │   ├── Bernoulli.cpp/hpp     # Distribution de Bernoulli
│   │   ├── Binomial.cpp/hpp      # Distribution binomiale
│   │   ├── Poisson.cpp/hpp       # Distribution de Poisson
│   │   ├── Uniform*.cpp/hpp      # Distributions uniformes (discrète/continue)
│   │   ├── Normal.cpp/hpp        # Distribution normale (Gauss)
│   │   ├── Exponential.cpp/hpp   # Distribution exponentielle
│   │   ├── Cauchy.cpp/hpp        # Distribution de Cauchy
│   │   ├── ChiSquared.cpp/hpp    # Distribution chi-carré
│   │   └── Hypergeometric.cpp/hpp# Distribution hypergéométrique
│   │
│   └── glimac/                   # Bibliothèque graphique personnalisée
│       └── src/glimac/           # Utilitaires OpenGL
│
├── lib/                          # Dépendances externes
│   ├── quick_imgui/              # ImGui avec glfw/opengl
│   ├── glm/                      # Mathématiques vectorielles (GLM)
│   └── tinygltf/                 # Loader GLTF
│
├── assets/                       # Ressources de jeu
│   ├── fonts/                    # Polices ImGui
│   ├── models/                   # Modèles GLTF 3D
│   └── shaders/                  # Shaders OpenGL
│       ├── blinnphong.vs/fs.glsl # Vertex/Fragment shaders (Blinn-Phong)
│       ├── board.vs/fs.glsl      # Shaders du plateau
│       └── skybox.vs/fs.glsl     # Shaders de la skybox
│
├── bin/                          # Exécutables compilés
│   └── Release/                  # Build de release
│
├── build/                        # Fichiers de compilation (CMake)
│
└── CMakeLists.txt                # Configuration CMake

```

### Modules principaux

#### **core/GameState**

- Gère l'état complet du jeu (pièces, plateau, tours)
- Valide les mouvements légaux
- Détecte check/checkmate/stalemate
- Enregistre l'historique des événements

#### **core/Board**

- Représente le plateau 8×8
- Gère les pièces et leurs positions
- Fournit des fonctions de requête (case occupée, pièce à position, etc.)

#### **core/Piece & pieces/**

- Classe abstraite `Piece` avec interface commune
- Implémentations concrètes pour chaque type de pièce
- Méthodes `getMoves()` pour calculer les coups possibles

#### **core/ChaoticEvent**

- Manager centralisé des 8 événements chaotiques
- Utilise les distributions mathématiques correspondantes
- Applique les transformations au plateau

#### **math/Distributions**

- Implémentations de 10 distributions de probabilité
- Interface commune pour générer des valeurs aléatoires
- Utilisées par `ChaoticEvent` pour les paramètres

#### **ui/Renderer**

- Rendu du plateau avec ImGui
- Affichage des pièces, sélections, coups possibles
- Interface utilisateur (historique, menus)

#### **ui/Renderer3D**

- Rendu OpenGL 3D du plateau
- Gestion de la caméra interactive
- Skybox et éclairage Blinn-Phong

#### **ui/InputHandler**

- Détecte les clics souris sur les cases
- Convertit positions écran → positions plateau
- Gère les contrôles caméra (clavier/souris)

---

## Installation et compilation

### Prérequis

- **Compilateur C++20** : GCC 10+, Clang 12+, ou MSVC 2019+
- **CMake** 3.20+
- **OpenGL** 3.3+ (driver graphique compatible)
- **Linux/macOS/Windows** avec support X11/Wayland (Linux), Cocoa (macOS), ou Win32 (Windows)

### Dépendances système

#### Sur Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake \
    libglfw3-dev libglad-dev \
    libfreetype6-dev
```

#### Sur macOS

```bash
brew install cmake glfw3 freetype
```

#### Sur Windows

- Installer Visual Studio 2019+ ou MinGW
- CMake sera utilisé pour la génération de projet

### Compilation

1. **Cloner le repository**

```bash
git clone <repository-url>
cd chess_imac_s4
```

2. **Créer le dossier de build**

```bash
mkdir build
cd build
```

3. **Générer avec CMake**

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

4. **Compiler**

```bash
cmake --build . --config Release
```

ou avec make directement :

```bash
make
```

5. **Exécuter**

```bash
cd ../bin/Release
./ImGuiTemplate
```

### Options de compilation

```bash
# Debug avec symboles
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release optimisé
cmake .. -DCMAKE_BUILD_TYPE=Release

# Avec support des avertissements stricts (comenté par défaut)
# cmake .. -DENABLE_WARNINGS_AS_ERRORS=ON
```

---

## Dépendances

### Bibliothèques intégrées

| Bibliothèque    | Version | Utilisation                             |
| --------------- | ------- | --------------------------------------- |
| **quick_imgui** | Latest  | Interface graphique ImGui               |
| **GLM**         | Latest  | Mathématiques vectorielles/matricielles |
| **TinyGLTF**    | Latest  | Chargement de modèles GLTF              |
| **GLAD**        | 0.1.34  | Chargement des fonctions OpenGL         |
| **GLFW**        | 3.3+    | Fenêtre et contexte OpenGL              |
| **ImGui**       | 1.89+   | Interface utilisateur                   |

### Dépendances système

- **OpenGL** 3.3+
- **C++ Standard Library** (C++20)

---

## Licence

Ce projet est développé dans le cadre du cours **Programmation Algorithmique** à l'IMAC.

Les dépendances utilisent leurs propres licences :

- **ImGui** : MIT
- **GLM** : Happy Bunny / Modified MIT
- **TinyGLTF** : MIT
- **GLAD** : MIT

---
