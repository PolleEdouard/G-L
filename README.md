# Projet Grammaire et Langage

Ce projet contient des programmes liés à la grammaire et aux langages.

## Prérequis

Avant de démarrer le projet, assurez-vous d'avoir installé les dépendances suivantes :

*   **GCC** : Le compilateur C.
*   **Make** : L'outil d'automatisation de la compilation.
*   **Graphviz** : Pour générer les visualisations graphiques des automates (commandes `dot`).

### Installation des dépendances

Voici les commandes pour installer ces outils sur les distributions Linux courantes :

**Debian / Ubuntu :**
```bash
sudo apt update
sudo apt install gcc make graphviz
```

**Fedora :**
```bash
sudo dnf install gcc make graphviz
```

## Utilisation

Pour compiler et exécuter le projet, utilisez `make`.

### Commandes principales

Voici quelques commandes courantes pour démarrer :

*   `make` : Compile l'ensemble du projet (génère les exécutables dans `results/`).
*   `make run-afd` : Exécute l'automate déterministe avec un fichier d'exemple.
*   `make run-afn` : Exécute l'automate non-déterministe.
*   `make view` : Ouvre les images des automates générés (nécessite d'avoir exécuté un programme auparavant).
*   `make clean` : Nettoie les fichiers compilés.

> **Note :** Cette liste n'est pas exhaustive. De nombreuses autres options de compilation, d'exécution (avec fichiers personnalisés) et de nettoyage sont disponibles.
>
> Pour consulter la liste complète des commandes, lancez :
> ```bash
> make help
> ```
