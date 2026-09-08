gcc main.c -o mon_programme

./mon-programme

# TP Structures de Données

**Arthur ZACHARY**
**DEV M2 - Groupe 2**

**Environnement :** Mac, architecture ARM64, compilateur GCC.

---

## 1. Complexités théoriques

Voici le tableau récapitulatif des complexités temporelles algorithmiques (notation Grand O) pour les structures implémentées.

| Opération | Dynamic Array | Linked List | Hash Table (Moyenne) | Hash Table (Pire cas) |
| :--- | :--- | :--- | :--- | :--- |
| **get(index)** | $O(1)$ | $O(n)$ | N/A | N/A |
| **find(value)** | $O(n)$ | $O(n)$ | $O(1)$ | $O(n)$ |
| **insert_front** | $O(n)$ | $O(1)$ | $O(1)$ | $O(1)$ |
| **insert_back** | $O(1)$ (amorti) | $O(n)$ | N/A | N/A |
| **remove_front**| $O(n)$ | $O(1)$ | $O(1)$ | $O(1)$ |
| **Parcours complet**| $O(n)$ | $O(n)$ | $O(n)$ | $O(n)$ |

---

## 2. Prédictions avant benchmark

Avant de lancer les mesures de performances, voici mes hypothèses basées sur la théorie algorithmique :
* L'accès par index (`get`) sera instantané sur le Dynamic Array, mais de plus en plus lent sur la Linked List à mesure que la taille $N$ augmente.
* L'insertion en tête (`insert_front`) sera le point faible majeur du Dynamic Array pour les grandes tailles (décalage de toute la mémoire), alors qu'elle sera instantanée pour la Linked List.
* La recherche d'une valeur absente sera lente en $O(n)$ pour l'Array et la List, mais quasiment instantanée pour la Hash Table avec une bonne fonction de hachage.
* La mauvaise fonction de hachage (`hash_bad`) transformera la Hash Table en une simple liste chaînée, nuisant ses performances de recherche (passage de $O(1)$ à $O(n)$).

---

## 3. Résultats & Comparaison

k0la@MacBook-Pro-de-Arthur TP-structure-de-données % ./mon_programme            

### Tableau Dynamique (Dynamic Array)

| Opération | N = 1 000 | N = 10 000 | N = 100 000 | N = 1 000 000 |
| :--- | :--- | :--- | :--- | :--- |
| **Accès N/2** | 0.000000 s | 0.000000 s | 0.000000 s | 0.000000 s |
| **Rech. absente** | 0.000004 s | 0.000037 s | 0.000181 s | 0.001010 s |
| **Insert début** | 0.000006 s | 0.000037 s | 0.000125 s | 0.000863 s |
| **Insert fin** | 0.000000 s | 0.000000 s | 0.000000 s | 0.000000 s |
| **Parcours somme** | 0.000005 s | 0.000028 s | 0.000108 s | 0.000752 s |

### Liste Chaînée (Linked List)

| Opération | N = 1 000 | N = 10 000 | N = 100 000 | N = 1 000 000 |
| :--- | :--- | :--- | :--- | :--- |
| **Accès N/2** | 0.000004 s | 0.000019 s | 0.000081 s | 0.000630 s |
| **Rech. absente** | 0.000007 s | 0.000033 s | 0.000116 s | 0.001080 s |
| **Insert début** | 0.000000 s | 0.000000 s | 0.000000 s | 0.000000 s |
| **Insert fin** | 0.000008 s | 0.000037 s | 0.000113 s | 0.001232 s |
| **Parcours somme** | 0.000008 s | 0.000029 s | 0.000120 s | 0.001124 s |

### Table de Hachage (Recherche d'une valeur absente)

| Fonction de hachage | N = 1 000 | N = 10 000 | N = 100 000 | N = 1 000 000 |
| :--- | :--- | :--- | :--- | :--- |
| **Hash GOOD** | 0.000000 s | 0.000000 s | 0.000000 s | 0.000000 s |
| **Hash BAD** | 0.000007 s | 0.000026 s | 0.000088 s | 0.001075 s |

**Analyse des résultats :**
* **Array vs List :** Les mesures confirment que le tableau dynamique est tres performant en lecture séquentielle (grâce à la localité de cache) et en accès direct. En revanche, l'insertion au début d'un tableau d'un million d'éléments est tres lourde, là où la liste ne subit aucun ralentissement.
* **Hash Good vs Hash Bad :** La différence est flagrante. La fonction `hash_good` permet de répartir uniformément les éléments dans les buckets, maintenant un temps de recherche indétectable. La fonction `hash_bad` force toutes les collisions dans un seul bucket, ce qui donne un temps de recherche catastrophique équivalent à celui de la liste chaînée.

---

## 4. Recommandation

### Situation A
**Contexte :** Collection rarement modifiée, mais 1 000 000 de recherches à effectuer.
**Choix :** Table de Hachage (Hash Table).
**Justification :** Le temps de recherche est le critère prioritaire ici. La table de hachage offre un temps d'accès moyen en $O(1)$, ce qui rendra le million de requêtes quasi instantané. Bien que cette structure consomme beaucoup de mémoire et que son coût de construction initial soit non négligeable, le fait que la collection soit "rarement modifiée" rentabilise largement cet investissement.

### Situation B
**Contexte :** Mémoire très limitée, seulement 10 recherches prévues.
**Choix :** Tableau Dynamique (Dynamic Array).
**Justification :** Le critère prioritaire est l'économie de mémoire. Le tableau dynamique est la structure la plus compacte : les données sont contiguës et n'exigent aucun pointeur supplémentaire (contrairement à la liste ou à la table, où chaque pointeur alourdit l'empreinte mémoire). Le compromis sur la performance de recherche en $O(n)$ est ici tout à fait acceptable, car un parcours de tableau pour seulement 10 recherches s'exécutera en une fraction de milliseconde, de manière totalement transparente pour l'utilisateur.