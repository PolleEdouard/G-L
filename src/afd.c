#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ETATS 100

// Structure pour un AFD
typedef struct {
  int n;        // nombre d'états
  int initial;  // état initial
  int *final;   // final[i] = 1 si état i est final
  int *delta_a; // transition par 'a'
  int *delta_b; // transition par 'b'
} AFD;

// Alloue un AFD avec n états
AFD *afd_new(int n) {
  AFD *a = malloc(sizeof(AFD));
  a->n = n;
  a->initial = 0;
  a->final = calloc(n, sizeof(int));
  a->delta_a = malloc(n * sizeof(int));
  a->delta_b = malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    a->delta_a[i] = -1;
    a->delta_b[i] = -1;
  }
  return a;
}

void afd_free(AFD *a) {
  if (a) {
    free(a->final);
    free(a->delta_a);
    free(a->delta_b);
    free(a);
  }
}

// Charge un AFD depuis un fichier
AFD *afd_load(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (!f) {
    printf("Erreur: impossible d'ouvrir %s\n", filename);
    return NULL;
  }

  int n;
  fscanf(f, "%d", &n);
  AFD *a = afd_new(n);

  fscanf(f, "%d", &a->initial);

  int nb_finals;
  fscanf(f, "%d", &nb_finals);
  for (int i = 0; i < nb_finals; i++) {
    int q;
    fscanf(f, "%d", &q);
    a->final[q] = 1;
  }

  int src, dst;
  char sym;
  while (fscanf(f, "%d %c %d", &src, &sym, &dst) == 3) {
    if (sym == 'a')
      a->delta_a[src] = dst;
    else if (sym == 'b')
      a->delta_b[src] = dst;
  }

  fclose(f);
  return a;
}

// Affiche un AFD
void afd_print(AFD *a, const char *titre) {
  printf("\n=== %s ===\n", titre);
  printf("Etats: %d, Initial: q%d\n", a->n, a->initial);
  printf("Finaux: {");
  for (int i = 0; i < a->n; i++)
    if (a->final[i])
      printf(" q%d", i);
  printf(" }\n");
  printf("Transitions:\n");
  for (int i = 0; i < a->n; i++) {
    printf("  q%d: ", i);
    if (a->delta_a[i] >= 0)
      printf("a->q%d ", a->delta_a[i]);
    if (a->delta_b[i] >= 0)
      printf("b->q%d", a->delta_b[i]);
    printf("\n");
  }
}

// Supprime les états inaccessibles (BFS depuis l'initial)
AFD *eliminer_inaccessibles(AFD *a) {
  int *accessible = calloc(a->n, sizeof(int));
  int *file = malloc(a->n * sizeof(int));
  int debut = 0, fin = 0;

  // BFS
  file[fin++] = a->initial;
  accessible[a->initial] = 1;

  while (debut < fin) {
    int q = file[debut++];
    int succ_a = a->delta_a[q];
    int succ_b = a->delta_b[q];
    if (succ_a >= 0 && !accessible[succ_a]) {
      accessible[succ_a] = 1;
      file[fin++] = succ_a;
    }
    if (succ_b >= 0 && !accessible[succ_b]) {
      accessible[succ_b] = 1;
      file[fin++] = succ_b;
    }
  }

  // Compter et renuméroter
  int *nouveau_num = malloc(a->n * sizeof(int));
  int nb_access = 0;
  for (int i = 0; i < a->n; i++) {
    if (accessible[i])
      nouveau_num[i] = nb_access++;
    else
      nouveau_num[i] = -1;
  }

  printf("Etats accessibles: %d/%d\n", nb_access, a->n);

  // Créer le nouvel AFD
  AFD *b = afd_new(nb_access);
  b->initial = nouveau_num[a->initial];

  for (int i = 0; i < a->n; i++) {
    if (!accessible[i])
      continue;
    int ni = nouveau_num[i];
    b->final[ni] = a->final[i];
    if (a->delta_a[i] >= 0)
      b->delta_a[ni] = nouveau_num[a->delta_a[i]];
    if (a->delta_b[i] >= 0)
      b->delta_b[ni] = nouveau_num[a->delta_b[i]];
  }

  free(accessible);
  free(file);
  free(nouveau_num);
  return b;
}

// Minimisation par partition
AFD *minimiser(AFD *a) {
  int *groupe = malloc(a->n * sizeof(int));
  int nb_groupes;

  // Partition initiale: finaux vs non-finaux
  int a_final = 0, a_non_final = 0;
  for (int i = 0; i < a->n; i++) {
    groupe[i] = a->final[i] ? 1 : 0;
    if (a->final[i])
      a_final++;
    else
      a_non_final++;
  }
  nb_groupes = (a_final > 0 && a_non_final > 0) ? 2 : 1;

  printf("Partition initiale: %d groupes\n", nb_groupes);

  // Raffinement
  bool change = true;
  while (change) {
    change = false;
    for (int g = 0; g < nb_groupes; g++) {
      // Collecter les états du groupe g
      int etats[MAX_ETATS], nb = 0;
      for (int i = 0; i < a->n; i++)
        if (groupe[i] == g)
          etats[nb++] = i;

      if (nb <= 1)
        continue;

      // Calculer les signatures (groupe_dest_a, groupe_dest_b)
      int sigs[MAX_ETATS];
      for (int i = 0; i < nb; i++) {
        int q = etats[i];
        int ga = (a->delta_a[q] >= 0) ? groupe[a->delta_a[q]] : -1;
        int gb = (a->delta_b[q] >= 0) ? groupe[a->delta_b[q]] : -1;
        sigs[i] = (ga + 1) * (nb_groupes + 2) + (gb + 1);
      }

      // Vérifier si division nécessaire
      int ref_sig = sigs[0];
      bool divise = false;
      for (int i = 1; i < nb; i++)
        if (sigs[i] != ref_sig) {
          divise = true;
          break;
        }

      if (!divise)
        continue;

      // Diviser le groupe
      change = true;
      int nouveau_g = nb_groupes;
      for (int i = 0; i < nb; i++) {
        if (sigs[i] != ref_sig) {
          groupe[etats[i]] = nouveau_g;
        }
      }
      nb_groupes++;
    }
  }

  printf("Partition finale: %d groupes\n", nb_groupes);

  // Construire l'AFD minimal
  AFD *m = afd_new(nb_groupes);
  m->initial = groupe[a->initial];

  for (int g = 0; g < nb_groupes; g++) {
    // Trouver un représentant du groupe
    int rep = -1;
    for (int i = 0; i < a->n; i++)
      if (groupe[i] == g) {
        rep = i;
        break;
      }

    m->final[g] = a->final[rep];
    if (a->delta_a[rep] >= 0)
      m->delta_a[g] = groupe[a->delta_a[rep]];
    if (a->delta_b[rep] >= 0)
      m->delta_b[g] = groupe[a->delta_b[rep]];
  }

  free(groupe);
  return m;
}

// Export DOT pour Graphviz
void exporter_dot(AFD *a, const char *filename) {
  FILE *f = fopen(filename, "w");
  if (!f)
    return;

  fprintf(f, "digraph AFD {\n  rankdir=LR;\n");
  fprintf(f, "  start [shape=none,label=\"\"];\n");
  fprintf(f, "  start -> q%d;\n", a->initial);

  for (int i = 0; i < a->n; i++) {
    fprintf(f, "  q%d [shape=%s];\n", i,
            a->final[i] ? "doublecircle" : "circle");
  }
  for (int i = 0; i < a->n; i++) {
    if (a->delta_a[i] >= 0)
      fprintf(f, "  q%d -> q%d [label=\"a\"];\n", i, a->delta_a[i]);
    if (a->delta_b[i] >= 0)
      fprintf(f, "  q%d -> q%d [label=\"b\"];\n", i, a->delta_b[i]);
  }
  fprintf(f, "}\n");
  fclose(f);
  printf("Fichier DOT: %s\n", filename);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <fichier_afd.txt>\n", argv[0]);
    printf(
        "Format: nb_etats\\n initial\\n nb_finals f1 f2...\\n transitions\n");
    return 1;
  }

  // Chargement
  AFD *afd = afd_load(argv[1]);
  if (!afd)
    return 1;
  afd_print(afd, "AFD Original");
  exporter_dot(afd, "results/afd_original.dot");

  // Élimination des inaccessibles
  printf("\n--- Elimination des etats inaccessibles ---\n");
  AFD *accessible = eliminer_inaccessibles(afd);
  afd_print(accessible, "AFD sans inaccessibles");

  // Minimisation
  printf("\n--- Minimisation ---\n");
  AFD *minimal = minimiser(accessible);
  afd_print(minimal, "AFD Minimal");
  exporter_dot(minimal, "results/afd_minimal.dot");

  // Résumé
  printf("\n=== Resume ===\n");
  printf("Original: %d etats -> Minimal: %d etats\n", afd->n, minimal->n);

  // Génération des images
  system("dot -Tpng results/afd_original.dot -o results/afd_original.png "
         "2>/dev/null");
  system("dot -Tpng results/afd_minimal.dot -o results/afd_minimal.png "
         "2>/dev/null");

  afd_free(afd);
  afd_free(accessible);
  afd_free(minimal);

  return 0;
}
