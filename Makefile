# Nom du compilateur
CC=gcc

# Options de compilation
CFLAGS=-Wall -Wextra -g -std=c11

# Dossier source et de résultats
SRCDIR := src
BINDIR := results

# Trouve tous les fichiers .c et génère les noms des exécutables
SOURCES := $(wildcard $(SRCDIR)/*.c)
EXECUTABLES := $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SOURCES))

# La cible 'all' est la cible par défaut.
# Elle dépend de tous les exécutables.
all: $(EXECUTABLES)

# Règle générique pour créer un exécutable dans $(BINDIR)
# à partir d'un fichier source de $(SRCDIR).
$(BINDIR)/%: $(SRCDIR)/%.c
	@mkdir -p $(BINDIR)
	@echo "Compilation de $< vers $@"
	$(CC) $(CFLAGS) $< -o $@

# Cible pour nettoyer les fichiers générés
clean:
	@echo "Nettoyage des fichiers générés..."
	@rm -rf $(BINDIR)
	@echo "Nettoyage terminé."

# Cible pour exécuter le programme
run: all
	@echo "Lancement du programme"
	@$(EXECUTABLES)

# Cible pour nettoyer, recompiler et exécuter le programme
clean-run: clean run

help:
	@echo "Utilisation:"
	@echo "  make <command>"
	@echo ""
	@echo "Commandes disponibles:"
	@echo "  all       : Compile tous les programmes sources."
	@echo "  clean     : Nettoie les fichiers binaires générés."
	@echo "  run       : Compile et exécute le programme principal."
	@echo "  clean-run : Nettoie, recompile et exécute le programme."
	@echo "  help      : Affiche ce message d'aide."

# Indique que 'all' et 'clean' ne sont pas des noms de fichiers.
.PHONY: all clean help run clean-run
