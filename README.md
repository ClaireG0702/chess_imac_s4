# Jeu d'échec

## Evenements aléatoires

Voici une liste des évènements aléatoire qu'il est possible de retrouver :

- **Révolution** : les pions se révoltent et passe du côté opposé
- **Swap Reine Pion** : les pions deviennent des dames et les dames de pions
- **Cavalier Sauvage** : une pièce au hasard devient un cavalier
<!-- - **Médusa** : une pièce se change en tour qui ne peut plus bouger -->
- **Rupture d'attention** : pendant 2 tours, les pièces ne respectent pas ce qui est dit et font un autre mouvement
- **Fuite** : les pions ne veulent plus aller se coller aux autres pions, les seuls mouvements possible durant ce tour c'est aller sur une case isolée
- **Dyscalculie** : les pions arrivent plus a bouger du bon nombre de case, tu leurs dis de bouger de 2 et ils bougent de 1 ou 3 au pif
- **Mutation** : une pièce change a un tour random de type
  <!-- - **Allier** : une pièce random devient neutre, elle joue pour la personne qui la bouge pendant les 3 prochains tours -->
  <!-- - **Fusion** : fusionne 2 pions et en fait un super pion qui peut se déplacer de 2 cases à chaque fois -->
- **Daltonien** : le plateau devient entièrement gris et tu peux jouer n'importe quelle pièce (au risque de se tromper)
  <!-- - **Reproduction** : si y'a 2 pions sur une meme ligne a 1 case d'écart alors rajoute un pion au mileu -->
  <!-- - **Shuffle** : on agite le plateau et on met les pièces au hasard dessus -->

## Tableau d'association des événements

| Evènement         | Loi proposée             | Pourquoi                                                                                                           |
| ----------------- | ------------------------ | ------------------------------------------------------------------------------------------------------------------ |
| Révolution        | **Loi binomiale**        | Chaque pion a une probabilité (p) de se rebeller. Le nombre de pions qui changent de camp suit donc une binomiale. |
| Swap Reine Pion   | **Loi de Poisson**       | Les Swap Reine Pions sont souvent modélisées par Poisson : nombre de transformations par tour.                            |
| Cavalier Sauvage  | **Loi uniforme**         | On choisit une pièce au hasard parmi toutes les pièces.                                                            |
| Rupture d'attention| **Loi de Cauchy**        | Loi très imprévisible avec valeurs extrêmes → correspond bien à un comportement chaotique.                         |
| Fuite           | **Loi hypergéométrique** | On sélectionne parmi les cases isolées sans remise dans un ensemble fini (le plateau).                             |
| Dyscalculie       | **Loi normale (Gauss)**  | L’erreur autour du mouvement prévu peut suivre une distribution centrée (1,2,3 cases autour de la valeur).         |
| Mutation       | **Loi exponentielle**    | Temps d'attente avant le changement de type de pièce.                                                              |
| Daltonien         | **Loi du Chi-deux**      | Utilisable pour mesurer la confusion entre catégories (types de pièces).                                           |

## Architecture des événements

Les événements sont séparés en deux catégories :

### Événements affectant le JEU (modifient l'état global du plateau)

- **Révolution** : les pions se révoltent et changent de côté
- **Swap Reine Pion** : les pions deviennent des dames et les dames deviennent des pions
- **Cavalier Sauvage** : une pièce au hasard devient un cavalier
- **Daltonien** : les pions deviennent gris, on peut jouer n'importe quelle pièce que ce soit de son camp ou du camp adversaire (tout en respectant les mouvements possibles)

### Événements affectant les PIONS (modifient le comportement des mouvements)

- **Rupture d'attention** : les pièces font un autre mouvement que celui choisi
- **Fuite** : les pions ne peuvent se déplacer que vers des cases isolées
- **Dyscalculie** : les pions bougent du mauvais nombre de cases
- **Mutation** : une pièce change aléatoirement de type

---

## Documentation Académique des Lois de Variables Aléatoires

Cette section documente rigoureusement l'ensemble des lois utilisées, leurs paramètres, l'espérance et la variance.

### Vérification des Critères du Projet

✅ **Critère i** : Toutes les lois sont différentes (8 lois distinctes)  
✅ **Critère ii** : Toutes les lois sont connues et bien établies en théorie des probabilités  
✅ **Critère iii** : 4 lois à densité (Cauchy, Normale, Exponentielle, Chi-Deux)  
✅ **Critère iv** : Tous les paramètres sont documentés et interprétés  
✅ **Critère v** : Espérance et variance documentées pour toutes les VA numériques

---

### 1. **Révolution** → Loi Binomiale $\mathcal{B}(n, p)$

**Type** : Loi discrète

**Paramètres** :

- $n$ = nombre de pions du joueur courant (n ≤ 16)
- $p$ = probabilité de rébellion par pion (recommandé: $p = 0.25$)

**Formule** : $X \sim \mathcal{B}(n, p)$

**Espérance** : $E[X] = np$

**Variance** : $\text{Var}(X) = np(1-p)$

**Justification** : Chaque pion a indépendamment une probabilité $p$ de se rebeller et changer de camp. Le nombre total de pions révoltés suit une binomiale.

**Exemple d'exécution** : Avec $n = 8$ pions et $p = 0.25$ :

- $E[X] = 8 \times 0.25 = 2$ pions changent de camp en moyenne
- $\text{Var}(X) = 8 \times 0.25 \times 0.75 = 1.5$

---

### 2. **Swap Reine Pion** → Loi de Poisson $\mathcal{P}(\lambda)$

**Type** : Loi discrète

**Paramètre** :

- $\lambda$ = taux moyen de transformations (recommandé: $\lambda = 2$)

**Formule** : $X \sim \mathcal{P}(\lambda)$, avec $P(X = k) = \frac{e^{-\lambda} \lambda^k}{k!}$

**Espérance** : $E[X] = \lambda$

**Variance** : $\text{Var}(X) = \lambda$

**Justification** : Les Swap Reine Pions suivent classiquement une loi de Poisson. Le nombre de transformations Pions ↔ Dames par événement est modélisé ainsi.

**Exemple d'exécution** : Avec $\lambda = 2$ :

- $E[X] = 2$ transformations en moyenne
- $\text{Var}(X) = 2$
- Les transformations sont indépendantes et rares (propriété de Poisson)

---

### 3. **Cavalier Sauvage** → Loi Uniforme Discrète $\mathcal{U}(a, b)$

**Type** : Loi discrète

**Paramètres** :

- $a = 1$ (première pièce)
- $b$ = nombre total de pièces non-rois (varie: 16-32 selon la phase du jeu)

**Formule** : $X \sim \mathcal{U}(a, b)$ avec $X \in \{1, 2, \ldots, b\}$

**Espérance** : $E[X] = \frac{a + b}{2}$

**Variance** : $\text{Var}(X) = \frac{(b - a + 1)^2 - 1}{12}$

**Justification** : La pièce est choisie uniformément aléatoirement parmi toutes les pièces présentes (sauf les rois, qui ne peuvent pas devenir cavaliers).

**Exemple d'exécution** : Avec $b = 30$ pièces non-rois :

- $E[X] = \frac{1 + 30}{2} = 15.5$
- $\text{Var}(X) = \frac{31^2 - 1}{12} = 80$

---

### 4. **Rupture d'attention** → Loi de Cauchy $\mathcal{C}(x_0, \gamma)$ ⭐ Densité

**Type** : Loi continue **à densité**

**Paramètres** :

- $x_0$ = localisation (centrage du mouvement, recommandé: $x_0 = 0$)
- $\gamma$ = échelle (variabilité, recommandé: $\gamma = 2$)

**Densité de probabilité** :
$$f(x) = \frac{1}{\pi \gamma \left[1 + \left(\frac{x - x_0}{\gamma}\right)^2\right]}$$

**Espérance** : **Non définie** ⚠️ (intégrale divergente)

**Variance** : **Infinie** ⚠️

**Justification** : La loi de Cauchy est extrêmement imprévisible avec des valeurs aberrantes infinies. Elle modélise parfaitement le Rupture d'attention où le mouvement effectué est radicalement différent de celui attendu, sans moyenne bien définie ni variance finie.

**Remarque académique** : C'est une loi pathologique rare où $E[X]$ et $\text{Var}(X)$ n'existent pas, illustrant l'imprévisibilité radicale.

---

### 5. **Fuite** → Loi Hypergéométrique $\mathcal{H}(N, K, n)$

**Type** : Loi discrète

**Paramètres** :

- $N$ = nombre total de cases du plateau = 64
- $K$ = nombre de cases isolées (sans pièces adjacentes) ≈ 20-30
- $n$ = nombre de destinations possibles pour la pièce ≈ 4-8

**Formule** : $X \sim \mathcal{H}(N, K, n)$, avec $P(X = k) = \frac{\binom{K}{k}\binom{N-K}{n-k}}{\binom{N}{n}}$

**Espérance** : $E[X] = n \cdot \frac{K}{N}$

**Variance** : $\text{Var}(X) = n \cdot \frac{K}{N} \cdot \frac{N-K}{N} \cdot \frac{N-n}{N-1}$

**Justification** : On sélectionne $n$ cases (déplacements possibles) parmi $N$ sans remise, et on compte combien sont "isolées" (succès = K). Tirage sans remise → hypergéométrique.

**Exemple d'exécution** : Avec $N = 64$, $K = 25$, $n = 6$ :

- $E[X] = 6 \times \frac{25}{64} \approx 2.34$ destinations isolées
- $\text{Var}(X) = 6 \times \frac{25}{64} \times \frac{39}{64} \times \frac{58}{63} \approx 1.32$

---

### 6. **Dyscalculie** → Loi Normale (Gauss) $\mathcal{N}(\mu, \sigma^2)$ ⭐ Densité

**Type** : Loi continue **à densité**

**Paramètres** :

- $\mu$ = distance attendue de déplacement (ex: $\mu = 3$ cases)
- $\sigma$ = écart-type représentant l'erreur (recommandé: $\sigma = 0.8$)

**Densité de probabilité** :
$$f(x) = \frac{1}{\sigma\sqrt{2\pi}} \exp\left(-\frac{(x - \mu)^2}{2\sigma^2}\right)$$

**Espérance** : $E[X] = \mu$

**Variance** : $\text{Var}(X) = \sigma^2$

**Justification** : L'erreur de calcul de distance suit une distribution normale centrée autour de la distance attendue. Par le théorème central limite, les petites erreurs sont plus fréquentes.

**Exemple d'exécution** : Avec $\mu = 3$ cases, $\sigma = 0.8$ :

- $E[X] = 3$ cases (pions bougent en moyenne de 3 cases)
- $\text{Var}(X) = 0.64$
- Les déplacements oscillent typiquement entre 1 et 5 cases (intervalle $[\mu - 2\sigma, \mu + 2\sigma]$)

---

### 7. **Mutation** → Loi Exponentielle $\mathcal{E}(\lambda)$ ⭐ Densité

**Type** : Loi continue **à densité**

**Paramètre** :

- $\lambda$ = taux de changement (recommandé: $\lambda = 0.5$ tours⁻¹)

**Densité de probabilité** :
$$f(x) = \lambda e^{-\lambda x} \quad \text{pour } x \geq 0$$

**Espérance** : $E[X] = \frac{1}{\lambda}$

**Variance** : $\text{Var}(X) = \frac{1}{\lambda^2}$

**Justification** : La loi exponentielle modélise le temps d'attente avant un événement dans un processus de Poisson. Ici, c'est le nombre de tours avant que la pièce change aléatoirement de type.

**Exemple d'exécution** : Avec $\lambda = 0.5$ :

- $E[X] = \frac{1}{0.5} = 2$ tours en moyenne avant changement
- $\text{Var}(X) = \frac{1}{0.25} = 4$
- Propriété d'absence de mémoire : le changement ne dépend pas du passé

---

### 8. **Daltonien** → Loi du Chi-Deux $\chi^2(k)$ ⭐ Densité

**Type** : Loi continue **à densité**

**Paramètre** :

- $k$ = degrés de liberté = nombre de types de pièces distinguables = 5  
  (Pion, Cavalier, Fou, Tour, Dame)

**Définition** : $X \sim \chi^2(k)$ où $X = \sum_{i=1}^{k} Z_i^2$ avec $Z_i \sim \mathcal{N}(0,1)$ indépendants

**Densité de probabilité** :
$$f(x) = \frac{1}{2^{k/2} \Gamma(k/2)} x^{(k/2)-1} e^{-x/2}$$

**Espérance** : $E[X] = k$

**Variance** : $\text{Var}(X) = 2k$

**Justification** : La confusion daltonienne correspond à une combinaison de "carrés d'erreurs" dans la discrimination des couleurs. Avec $k=5$ types de pièces, la confusion suit une Chi-Deux(5).

**Exemple d'exécution** : Avec $k = 5$ :

- $E[X] = 5$ (niveau moyen de confusion)
- $\text{Var}(X) = 10$
- Loi asymétrique reflétant que la confusion peut être très faible ou très élevée

---

## Résumé Comparatif Complet

| Événement   | Loi              | Type     | Densité?   | $E[X]$ définie? | $\text{Var}(X)$ définie? |
| ----------- | ---------------- | -------- | ---------- | --------------- | ------------------------ |
| Révolution  | Binomiale        | Discrète | ❌ Non     | ✅ Oui          | ✅ Oui                   |
| SwapReinePion| Poisson          | Discrète | ❌ Non     | ✅ Oui          | ✅ Oui                   |
| Cavalier    | Uniforme disc.   | Discrète | ❌ Non     | ✅ Oui          | ✅ Oui                   |
| Rupture d'attention | Cauchy           | Continue | ✅ **Oui** | ❌ Non          | ❌ Non                   |
| Fuite     | Hypergéométrique | Discrète | ❌ Non     | ✅ Oui          | ✅ Oui                   |
| Dyscalculie | Normale          | Continue | ✅ **Oui** | ✅ Oui          | ✅ Oui                   |
| Mutation | Exponentielle    | Continue | ✅ **Oui** | ✅ Oui          | ✅ Oui                   |
| Daltonien   | Chi-Deux         | Continue | ✅ **Oui** | ✅ Oui          | ✅ Oui                   |

**Lois à densité (Critère iii)** : 4/8 ✅ (Cauchy, Normale, Exponentielle, Chi-Deux)
