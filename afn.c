#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_NFA_STATES 10      // Maximum d'états pour l'AFN
#define MAX_DFA_STATES 100     // Maximum d'états possibles pour l'AFD (2^N)
#define ALPHABET_SIZE 2        // a et b

// Structure pour l'AFN (Entrée)
// transition[etat_source][symbole][etat_dest] = 1 si transition existe
int nfa_trans[MAX_NFA_STATES][ALPHABET_SIZE][MAX_NFA_STATES];
int nfa_final[MAX_NFA_STATES];
int num_nfa_states = 0;

// Structure pour l'AFD (Sortie)
// Chaque état de l'AFD est un masque de bits (int) représentant un sous-ensemble d'états AFN
int dfa_states[MAX_DFA_STATES];     // Stocke le masque binaire de chaque état AFD
int dfa_trans[MAX_DFA_STATES][ALPHABET_SIZE]; // Table de transition de l'AFD
int dfa_final[MAX_DFA_STATES];      // 1 si l'état AFD est terminal
int num_dfa_states = 0;

// Pile/File pour traiter les nouveaux états AFD
int queue[MAX_DFA_STATES];
int front = 0;
int rear = 0;

// Fonction pour ajouter à la file
void enqueue(int mask) {
    queue[rear++] = mask;
}

// Fonction pour retirer de la file
int dequeue() {
    return queue[front++];
}

// Vérifie si la file est vide
bool is_queue_empty() {
    return front == rear;
}

// Trouve l'index d'un état AFD existant à partir de son masque. Retourne -1 si non trouvé.
int find_dfa_state_index(int mask) {
    for (int i = 0; i < num_dfa_states; i++) {
        if (dfa_states[i] == mask) {
            return i;
        }
    }
    return -1;
}

// Vérifie si un état composé (masque) contient au moins un état final de l'AFN
int check_is_final(int mask) {
    for (int i = 0; i < num_nfa_states; i++) {
        // Si le i-ème bit est à 1 et que l'état i est final dans l'AFN
        if ((mask & (1 << i)) && nfa_final[i]) {
            return 1;
        }
    }
    return 0;
}

void convert_nfa_to_dfa() {
    // 1. L'état initial de l'AFD est l'état {0} (donc masque binaire 1 << 0 = 1)
    int start_mask = (1 << 0);
    
    dfa_states[num_dfa_states] = start_mask;
    dfa_final[num_dfa_states] = check_is_final(start_mask);
    num_dfa_states++;
    
    enqueue(start_mask);

    // 2. Boucle principale
    while (!is_queue_empty()) {
        int current_mask = dequeue();
        int current_dfa_index = find_dfa_state_index(current_mask);

        // Pour chaque symbole ('a' = 0, 'b' = 1)
        for (int symbol = 0; symbol < ALPHABET_SIZE; symbol++) {
            int next_mask = 0;

            // On parcourt tous les états de l'AFN présents dans le masque actuel
            for (int i = 0; i < num_nfa_states; i++) {
                if (current_mask & (1 << i)) {
                    // Si l'état i est dans l'ensemble actuel, on regarde où il va
                    for (int j = 0; j < num_nfa_states; j++) {
                        if (nfa_trans[i][symbol][j] == 1) {
                            // On ajoute l'état j au masque de destination
                            next_mask |= (1 << j);
                        }
                    }
                }
            }

            // Si le résultat est un ensemble vide (puits), le masque est 0
            
            // On vérifie si cet état (next_mask) existe déjà dans l'AFD
            int existing_index = find_dfa_state_index(next_mask);
            
            if (existing_index == -1) {
                // Nouvel état découvert !
                existing_index = num_dfa_states;
                dfa_states[num_dfa_states] = next_mask;
                dfa_final[num_dfa_states] = check_is_final(next_mask);
                num_dfa_states++;
                enqueue(next_mask);
            }

            // On enregistre la transition dans la table de l'AFD
            dfa_trans[current_dfa_index][symbol] = existing_index;
        }
    }
}

void print_binary(int num, int states) {
    printf("{ ");
    for (int i = 0; i < states; i++) {
        if (num & (1 << i)) printf("q%d ", i);
    }
    printf("}");
}

int main() {
    // Lecture de l'entrée selon votre format
    // Format attendu par ligne: transitions A (N bits), transitions B (N bits), Flag (1 bit)
    
    printf("Entrez le nombre d'etats de l'AFN: ");
    if (scanf("%d", &num_nfa_states) != 1) return 1;

    printf("Entrez la matrice (Format: trans_A... trans_B... Flag):\n");
    printf("Exemple pour 4 etats: 1100  1000 0\n");

    for (int i = 0; i < num_nfa_states; i++) {
        // Lecture transitions 'a'
        for (int j = 0; j < num_nfa_states; j++) {
            scanf("%1d", &nfa_trans[i][0][j]);
        }
        // Lecture transitions 'b'
        for (int j = 0; j < num_nfa_states; j++) {
            scanf("%1d", &nfa_trans[i][1][j]);
        }
        // Lecture flag terminal
        scanf("%d", &nfa_final[i]);
    }

    printf("\n--- Conversion en cours ---\n");
    convert_nfa_to_dfa();

    printf("\n--- AUTOMATE FINI DETERMINISTE (AFD) RESULTANT ---\n");
    printf("Etat ID | Sous-ensemble | Trans 'a' | Trans 'b' | Terminal?\n");
    printf("-----------------------------------------------------------\n");

    for (int i = 0; i < num_dfa_states; i++) {
        printf("   %d    | ", i);
        
        // Affichage joli du sous-ensemble (ex: {q0, q1})
        print_binary(dfa_states[i], num_nfa_states);
        
        // Alignement
        int len = 0; // Estimation simple
        int temp = dfa_states[i];
        while(temp) { if(temp & 1) len+=3; temp >>= 1; }
        if (len < 12) for(int k=0; k<12-len; k++) printf(" ");

        printf("|    %d      |    %d      |    %s\n", 
            dfa_trans[i][0], 
            dfa_trans[i][1], 
            dfa_final[i] ? "OUI" : "NON");
    }

    return 0;
}