# =============================================================================
# Makefile pour le projet G-L (Grammaires et Langages)
# =============================================================================

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

# Dossiers
SRCDIR = src
BINDIR = results
EXDIR = examples

# Exécutables
AFD = $(BINDIR)/afd
AFN = $(BINDIR)/afn

# Trouve tous les fichiers sources
SOURCES = $(wildcard $(SRCDIR)/*.c)
EXECUTABLES = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SOURCES))

# =============================================================================
# Cibles principales
# =============================================================================

# Cible par défaut : compile tous les programmes
all: $(EXECUTABLES)

# Règle spécifique pour afd
afd: $(AFD)

$(AFD): $(SRCDIR)/afd.c
	@mkdir -p $(BINDIR)
	@echo "Compilation de afd..."
	$(CC) $(CFLAGS) $< -o $@
	@echo "✓ Exécutable créé: $@"

# Règle spécifique pour afn
afn: $(AFN)

$(AFN): $(SRCDIR)/afn.c
	@mkdir -p $(BINDIR)
	@echo "Compilation de afn..."
	$(CC) $(CFLAGS) $< -o $@
	@echo "✓ Exécutable créé: $@"

# Règle générique pour les autres fichiers sources
$(BINDIR)/%: $(SRCDIR)/%.c
	@mkdir -p $(BINDIR)
	@echo "Compilation de $<..."
	$(CC) $(CFLAGS) $< -o $@

# =============================================================================
# Exécution
# =============================================================================

# Exécuter afd avec le fichier d'exemple divisible_par_3
run-afd: $(AFD)
	@echo ""
	@echo "=== Exécution de afd avec divisible_par_3.txt ==="
	@$(AFD) $(EXDIR)/divisible_par_3.txt

# Exécuter afn
run-afn: $(AFN)
	@echo ""
	@echo "=== Exécution de afn ==="
	@$(AFN)

# Exécuter afd avec un fichier personnalisé (usage: make run FILE=mon_fichier.txt)
run: $(AFD)
ifdef FILE
	@$(AFD) $(FILE)
else
	@echo "Usage: make run FILE=<chemin_vers_fichier.txt>"
	@echo "Exemple: make run FILE=examples/divisible_par_3.txt"
endif

# =============================================================================
# Nettoyage
# =============================================================================

# Nettoyer les fichiers objets et exécutables
clean:
	@echo "Nettoyage des fichiers générés..."
	@rm -f $(BINDIR)/afd $(BINDIR)/afn
	@rm -f $(BINDIR)/*.o
	@echo "✓ Exécutables supprimés"

# Nettoyer aussi les fichiers DOT et PNG générés
clean-all: clean
	@echo "Nettoyage des fichiers de visualisation..."
	@rm -f $(BINDIR)/*.dot $(BINDIR)/*.png
	@echo "✓ Fichiers DOT et PNG supprimés"

# Nettoyer complètement le dossier results
clean-results:
	@echo "Suppression du dossier results..."
	@rm -rf $(BINDIR)
	@echo "✓ Dossier results supprimé"

# =============================================================================
# Visualisation
# =============================================================================

# Ouvrir les images générées
view: 
	@if [ -f $(BINDIR)/afd_original.png ]; then \
		echo "Ouverture de afd_original.png..."; \
		xdg-open $(BINDIR)/afd_original.png 2>/dev/null & \
	fi
	@if [ -f $(BINDIR)/afd_minimal.png ]; then \
		echo "Ouverture de afd_minimal.png..."; \
		xdg-open $(BINDIR)/afd_minimal.png 2>/dev/null & \
	fi

# =============================================================================
# Aide
# =============================================================================

help:
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════════╗"
	@echo "║            Makefile - Projet Grammaires et Langages           ║"
	@echo "╚════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Compilation:"
	@echo "  make           - Compile tous les programmes (afd, afn)"
	@echo "  make afd       - Compile uniquement le programme afd"
	@echo "  make afn       - Compile uniquement le programme afn"
	@echo ""
	@echo "Exécution:"
	@echo "  make run-afd   - Exécute afd avec examples/divisible_par_3.txt"
	@echo "  make run-afn   - Exécute afn (saisie interactive)"
	@echo "  make run FILE=<fichier> - Exécute afd avec un fichier spécifique"
	@echo ""
	@echo "Nettoyage:"
	@echo "  make clean         - Supprime les exécutables et fichiers .o"
	@echo "  make clean-all     - Supprime aussi les fichiers .dot et .png"
	@echo "  make clean-results - Supprime le dossier results entier"
	@echo ""
	@echo "Visualisation:"
	@echo "  make view      - Ouvre les images PNG générées"
	@echo ""
	@echo "Aide:"
	@echo "  make help      - Affiche ce message d'aide"
	@echo ""

# Indique les cibles qui ne sont pas des fichiers
.PHONY: all afd afn clean clean-all clean-results run run-afd run-afn view help
