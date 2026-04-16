# 🎲 Jeu d'Échecs Chaotique

Un jeu d'échecs augmenté avec des événements chaotiques générés par des lois de probabilité distinctes. Chaque événement transforme le gameplay de manière imprévisible mais contrôlée.

---

## 🎲 Les 8 Événements Chaotiques

### 🎲 **Révolution** — Des pions changent de camp

#### 🎮 Description du Jeu

De temps en temps, **2-3 pions (Pawn) changent de camp**. Ces pions, sélectionnés aléatoirement sans remise, deviennent des pions ennemis avec toutes les implications tactiques.

**Lois** : Binomiale + Hypergéométrique

#### 🧠 Pourquoi la Binomiale ?

La **Binomial distribution** modélise :

> **"Combien d'individus dans un groupe subissent un changement"**

C'est exactement le cas ici :

- Chaque pion a une probabilité `p` de "se rebeller"
- Tu veux un nombre variable mais contrôlé

**✔ Avantage gameplay** :

- Évite un résultat fixe (toujours 2 ou 3 pions)
- Produit naturellement des petits nombres (si p faible)
- Crée de la tension : "combien vont vraiment se rebeller ?"

#### 🧠 Pourquoi l'Hypergéométrique ?

La **Hypergeometric distribution** modélise :

> **Tirage sans remise dans une population finie**

Dans le jeu :

- Tu ne peux pas sélectionner deux fois le même pion
- Le plateau est un ensemble fini (16 pions max par camp)

**✔ Avantage gameplay** :

- Résultat cohérent (pas de doublons)
- Plus réaliste qu'un tirage indépendant
- Élimine les pions un par un jusqu'à atteindre le nombre requis

---

### 🔁 **Swap Reine ↔ Pion** — Des pions deviennent des dames et inversement

#### 🎮 Description du Jeu

De temps en temps, **un pion aléatoire est choisi parmi tous les Pawn et Queen**. Celui-ci change immédiatement pour devenir de l'autre type : un Pawn devient Queen, ou une Queen devient Pawn.

**Lois** : Poisson + Uniforme Discrète

#### 🧠 Pourquoi la Poisson ?

La **Poisson distribution** modélise :

> **Le nombre d'événements rares dans un intervalle**

Ici :

- L'événement est rare
- Tu veux qu'il arrive "de temps en temps", sans régularité

**✔ Avantage gameplay** :

- Imprévisible mais stable statistiquement
- Évite les patterns (genre "tous les 5 tours")
- Peut déclencher 0, 1, 2, 3+ transformations → variation naturelle

#### 🧠 Pourquoi l'Uniforme Discrète ?

La **Uniform distribution** :

> **Toutes les pièces ont la même chance**

✔ Gameplay :

- Pas de biais (sinon frustration)
- Facile à comprendre pour le joueur
- Toute pièce sur le plateau peut être transformée

---

### 🐎 **Cavalier Sauvage** — Une pièce au hasard devient un cavalier

#### 🎮 Description du Jeu

De temps en temps, **un pion aléatoire parmi tous les pions du plateau, sauf les King, est choisi et devient un Knight**. Cette transformation est instantanée et irréversible.

**Loi** : Exponentielle

#### 🧠 Pourquoi Exponentielle ?

La **Exponential distribution** modélise :

> **Le temps entre deux événements indépendants**

**Propriété clé** : 👉 **Absence de mémoire (memoryless)**

Ça veut dire :

- Peu importe depuis combien de temps rien ne s'est passé
- La probabilité reste la même

**✔ Gameplay** :

- Très naturel pour un événement "sauvage"
- Pas prévisible → tension constante
- Peut se déclencher "à tout moment"

---

### 🧠 **Dyscalculie** — Les pions bougent du mauvais nombre de cases

#### 🎮 Description du Jeu

De temps en temps, lorsqu'un joueur joue un pion, **le pion avance d'une case de plus ou de moins que le mouvement prévu**. Cette erreur de calcul peut sauver ou perdre une partie en une fraction de seconde.

**Loi** : Cauchy

#### 🧠 Pourquoi Cauchy ?

La **Cauchy distribution** est spéciale :

> **Elle a des queues très lourdes**

Cela signifie :

- Petites erreurs fréquentes
- **MAIS** grosses erreurs possibles (très possibles!)

Contrairement à la normale :

- Les extrêmes sont beaucoup plus probables

**✔ Gameplay** :

- Parfait pour un effet "erreur humaine"
- Génère des situations absurdes → fun chaotique
- Celles-ci étaient imprévisibles plutôt que juste "bruitées"

---

### 🧬 **Mutation** — Une pièce change de type aléatoirement

#### 🎮 Description du Jeu

De temps en temps, **au début du tour un pion aléatoire parmi ceux du joueur (sauf King) change de type**. Cette transformation affecte les possibilités de jeu pour ce tour et les tours suivants.

**Loi** : Chi-Carré

#### 🧠 Pourquoi Chi² ?

La **Chi-squared distribution** :

- Toujours positive
- Asymétrique (biaisée)
  > **Elle favorise certaines valeurs**

Dans le jeu :

- Certaines transformations peuvent être plus fréquentes que d'autres

**✔ Gameplay** :

- Permet de contrôler indirectement les probabilités
- Évite un chaos totalement uniforme (plus intéressant)
- Résultat peu prévisible mais cohérent

---

### 🧠 **Rupture d'Attention** — Le joueur fait un autre coup au hasard

#### 🎮 Description du Jeu

De temps en temps, lorsqu'un joueur joue, **son pion fait un autre des mouvements possibles que celui choisi par le joueur**. Le joueur voit son intention complètement déjouée, mais le mouvement reste légal.

**Loi** : Uniforme Discrète

#### 🧠 Pourquoi Uniforme discrète ?

La **Uniform distribution** :

> **Une fois l'erreur déclenchée → aucun biais**

✔ Logique :

- Tous les coups légaux sont également plausibles
- Pas de favoritisme caché

✔ Gameplay :

- Clair pour le joueur
- Pas de frustration liée à un biais caché
- L'aléatoire est le vrai problème, pas l'injustice

---

### 🏃 **Fuite** — Les pions ne peuvent aller que sur des cases isolées ET le tour s'arrête

#### 🎮 Description du Jeu

De temps en temps, **un pion va vers une case isolée (sans voisin autour)**. Le joueur ne peut pas continuer à jouer car un mouvement a déjà été réaliser : **fin de son tour**. Une pénalité brutale et irrévocable.

**Loi** : Exponentielle

#### 🧠 Pourquoi Exponentielle (encore) ?

Même logique que **Cavalier Sauvage** :

> **Événement rare + imprévisible dans le temps**

Mais ici :

- **Impact très fort** (fin de tour automatique)

✔ Gameplay :

- Effet "surprise brutale"
- Pas anticipable → chaos pur
- Pénalité importante qui le rend redouté

---

### 🎨 **Daltonisme** — Le plateau devient gris, tu peux jouer n'importe quelle pièce

#### 🎮 Description du Jeu

De temps en temps, **pendant 1 tour seulement, tous les pions du plateau deviennent gris**. Le joueur qui doit jouer peut jouer **tous les pions, même ceux censés être à l'autre joueur**. Une confusion totale mais temporaire.

**Loi** : Normale (Gauss)

#### 🧠 Pourquoi Normale ?

La **Normal distribution** modélise :

> **Des variations autour d'une moyenne**

Ici :

- Tu ne veux pas un chaos constant
- Tu veux des "pics" occasionnels (durée variable)

✔ Gameplay :

- Permet d'introduire un niveau de chaos fluctuant
- Facile à équilibrer avec μ (fréquence) et σ (variabilité)
- Certains tours sont plus "brumeux" que d'autres

---

## 📊 Résumé des Lois Utilisées

| 🎲 Événement     | 📐 Loi           | Type     | Propriétés                                 |
| ---------------- | ---------------- | -------- | ------------------------------------------ |
| 🎲 Révolution    | Binomiale + Hyp. | Discrète | Variables contrôlées, sans remise          |
| 🔁 SwapReinePion | Poisson + Unif.  | Discrète | Rareté naturelle + sélection équitable     |
| 🐎 Cavalier      | Exponentielle    | Continue | Absence de mémoire, imprévisible           |
| 🧠 Dyscalculie   | Cauchy           | Continue | Queues lourdes, erreurs extrêmes possibles |
| 🧬 Mutation      | Chi-Carré        | Continue | Asymétrique, certains types favorisés      |
| 🧠 Rupture       | Uniforme disc.   | Discrète | Équité totale dans la sélection            |
| 🏃 Fuite         | Exponentielle    | Continue | Imprévisibilité + impact fort              |
| 🎨 Daltonisme    | Normale          | Continue | Variation autour d'une moyenne             |

---

## 🎮 Stratégie de Gameplay

### 45% de déclencher un événement par tour (en mode Chaos)

Une fois l'événement sélectionné aléatoirement, sa loi de probabilité détermine :

- **Magnitude** : Combien de pions/cases sont affectés (Binomiale, Poisson, etc.)
- **Sévérité** : L'ampleur de l'impact (Cauchy pour extrêmes, Normale pour modéré)
- **Équité** : Comment on sélectionne (Uniforme pour pas de biais, Hypergéométrique pour cohérence)

### Les pièges à éviter

⚠️ **Revenir à l'Uniforme pour tout** : Les événements deviendraient prévisibles et ennuyeux  
⚠️ **Utiliser les mêmes lois** : Perte de variété et d'intérêt  
⚠️ **Ignorer les propres mathématiques des événements** : Le chaos perd du statut

---

## 🏗️ Architecture du Code

**Fichier unifié** : `ChaoticEvent.hpp/cpp`

- Tous les 8 événements en un seul manager
- Une méthode par événement avec sa distribution propre
- Intégration dans `GameState` pour exécution à chaque tour

**Historique des événements** :

- Format simplifié : `[Tour X] NomEvenement`
- Pas de couleur joueur (les événements sont globaux)

---

## 📚 Lectures Complémentaires

- Théorie des probabilités appliquée au gameplay
- Balancing aléatoire dans les jeux de stratégie
- Propriétés des lois continues vs discrètes
