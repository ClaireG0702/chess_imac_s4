# Jeu d'échec

## Evenements aléatoires

Voici une liste des évènements aléatoire qu'il est possible de retrouver :

- **Révolution** : les pions se révoltent et passe du côté opposé
- **Epidémie de trans** : les pions deviennent des dames et les dames de pions
- **Furry** : une pièce au hasard devient un cavalier
- **Médusa** : une pièce se change en tour qui ne peut plus bouger
- **TDAH** : pendant 2 tours, les pièces ne respectent pas ce qui est dit et font un autre mouvement
- **Autisme** : les pions ne veulent plus aller se coller aux autres pions, les seuls mouvements possible durant ce tour c'est aller sur une case isolée
- **Dyscalculie** : les pions arrivent plus a bouger du bon nombre de case, tu leurs dis de bouger de 2 et ils bougent de 1 ou 3 au pif
- **Genderfluid** : une pièce change a un tour random de type
- **Allier** : une pièce random devient neutre, elle joue pour la personne qui la bouge pendant les 3 prochains tours
- **Fusion** : fusionne 2 pions et en fait un super pion qui peut se déplacer de 2 cases à chaque fois
- **Daltonien** : le plateau devient entièrement gris et tu peux jouer n'importe quelle pièce (au risque de se tromper)
- **Reproduction** : si y'a 2 pions sur une meme ligne a 1 case d'écart alors rajoute un pion au mileu
- **Shuffle** : on agite le plateau et on met les pièces au hasard dessus

## Tableau d’association des évènements

| Evènement    | Loi proposée              | Pourquoi                                                                                                           |
| ------------ | ------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| Révolution   | **Loi binomiale**         | Chaque pion a une probabilité (p) de se rebeller. Le nombre de pions qui changent de camp suit donc une binomiale. |
| Epidémie     | **Loi de Poisson**        | Les épidémies sont souvent modélisées par Poisson : nombre de transformations par tour.                            |
| Furry        | **Loi uniforme**          | On choisit une pièce au hasard parmi toutes les pièces.                                                            |
| Médusa       | **Loi géométrique**       | On peut modéliser le nombre de tours avant qu’une pièce soit pétrifiée.                                            |
| TDAH         | **Loi de Cauchy**         | Loi très imprévisible avec valeurs extrêmes → correspond bien à un comportement chaotique.                         |
| Autisme      | **Loi hypergéométrique**  | On sélectionne parmi les cases isolées sans remise dans un ensemble fini (le plateau).                             |
| Dyscalculie  | **Loi normale (Gauss)**   | L’erreur autour du mouvement prévu peut suivre une distribution centrée (1,2,3 cases autour de la valeur).         |
| Genderfluid  | **Loi exponentielle**     | Temps d’attente avant le changement de type de pièce.                                                              |
| Allier       | **Loi bêta**              | Permet de modéliser une probabilité variable de loyauté pendant plusieurs tours.                                   |
| Fusion       | **Loi Gamma / Erlang**    | Peut modéliser le temps nécessaire avant que deux pions fusionnent.                                                |
| Daltonien    | **Loi du Chi-deux**       | Utilisable pour mesurer la confusion entre catégories (types de pièces).                                           |
| Reproduction | **Loi de Weibull**        | Utilisée pour modéliser des phénomènes de croissance ou d’apparition.                                              |
| Shuffle      | **Permutation aléatoire** | On mélange toutes les pièces sur les cases du plateau.                                                             |
