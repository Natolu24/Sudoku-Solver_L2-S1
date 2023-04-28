#include <stdio.h>
#include <stdlib.h>

typedef struct s_case_t {
    int val;
    struct s_case_t *nw, *n, *ne, *w, *e, *sw, *s, *se;
    int is_original;
} case_t;

/* Va lire le fichier donné pour ensuite créer et renvoyer la liste des cases de la ligne, dans le sens gauche droite, haut bas (81 cases donc) */
int read_grid(char * file_name, case_t* grid[81]);
/* Va lié deux cases entre elles */
void link(case_t** cas1, char direction, case_t** cas2);
/* Libère la grille */
void free_grid(case_t* grid[81]);
/* Va résoudre et compléter la grille, avec la méthode du backtracking */
void solve(case_t* grid[81]);
/* Affiche une grille de sudoku */
void print_grid(case_t* grid[81]);
/* Verification si la case respecte bien les chiffres de sa ligne*/
int check_line(case_t* grid[81], int x, case_t* cas, int direction);
/* Verification si la case respecte bien les chiffres de sa colonne*/
int check_colunm(case_t* grid[81], int x, case_t* cas, int direction);
/* Verification si la case respecte bien les chiffres de sa zone*/
int check_zone(case_t* grid[81], int x, case_t* cas);
/* Renvoie la position X de la case dans la grille (utilise les 3 fonctions suivantes pour y arrivé)*/
int get_posx(case_t* cas);
/* Renvoie la position Y de la case dans la grille */
int get_posy(case_t* cas);
/* Renvoie la case se situant au milieu de la zone d'une case donné*/
case_t* get_middle(case_t* cas, case_t* grid[81]);

int read_grid(char * file_name, case_t* grid[81]) {
    FILE* fichier = fopen(file_name, "r");
    int i, c;
    for (i = 0; i < 81; i++) {
        c = fgetc(fichier)-48;
        if (c == EOF)
            return 1;
        case_t* tmp = malloc(sizeof (*tmp));
        if ((c >= 1 && c <= 9) || c == 47) {
            if (c == 47) { // si pas de chiffre présent on met 0
                tmp->val = 0;
                tmp->is_original = 0;
            }
            else { // si chiffre présent on le marque, et sa sera un chiffre "original"
                tmp->val = c;
                tmp->is_original = 1;
            }
            if (i == 0) { // case du coin en haut a gauche
                tmp->w = NULL;
                tmp->nw = NULL;
                tmp->n = NULL;
                tmp->ne = NULL;
            }
            else if (i <= 8) { // autres cases en haut
                link(&grid[i-1], '-', &tmp);
                tmp->nw = NULL;
                tmp->n = NULL;
                tmp->ne = NULL;
            }
            else if (i%9 == 0) { // case de la première colonne (sauf celle de la première ligne)
                tmp->w = NULL;
                tmp->nw = NULL;
                link(&grid[i-9], '|', &tmp);
                link(&grid[i-8], '/', &tmp);
            }
            else if (i%9 == 8) { // case de la dernière colonne (sauf celle de la première ligne)
                link(&grid[i-1], '-', &tmp);
                link(&grid[i-10], '\\', &tmp);
                link(&grid[i-9], '|', &tmp);
                tmp->ne = NULL;
            }
            else { // les cases restantes
                link(&grid[i-1], '-', &tmp);
                link(&grid[i-10], '\\', &tmp);
                link(&grid[i-9], '|', &tmp);
                link(&grid[i-8], '/', &tmp);
            }
            tmp->e = NULL;
            tmp->sw = NULL;
            tmp->s = NULL;
            tmp->se = NULL;
            grid[i] = tmp;
        }
        else // mauvais caractère ? on passe au suivant
            i--;
    }
    return 0;
}

void link(case_t** cas1, char direction, case_t** cas2) {
    if (direction == '-') { // west & est
        (*cas1)->e = *cas2;
        (*cas2)->w = *cas1;
    }
    else if (direction == '\\') { // nord-west & south-est
        (*cas1)->se = *cas2;
        (*cas2)->nw = *cas1;
    }
    else if (direction == '|') { // nord & south
        (*cas1)->s = *cas2;
        (*cas2)->n = *cas1;
    }
    else if (direction == '/') { // south-west & nord-est
        (*cas1)->sw = *cas2;
        (*cas2)->ne = *cas1;
    }
}

void free_grid(case_t* grid[81]) {
    int i;
    for (i = 0; i < 81; i++) {
        free(grid[i]);
    }
}

void solve(case_t* grid[81]) {
    int i;
    for (i = 0; i < 81;) {
        if (!grid[i]->is_original) { // Si on est pas sur une case d'origine
            grid[i]->val++; // On incrémente
        }
        if (grid[i]->val == 10) { // Si on atteint la limite (10)
            grid[i]->val = 0; // On reset
            printf("Reset de la case au coordonées %d-%d\n", get_posx(grid[i]), get_posy(grid[i]));
            while (grid[--i]->is_original); // Et revient a la case précédente (n'étant pas d'origine)
            printf("Retour a la case au coordonées %d-%d\n", get_posx(grid[i]), get_posy(grid[i]));
            print_grid(grid);
        }
        else if ((!(check_line(grid, grid[i]->val, grid[i]->w, -1) || check_line(grid, grid[i]->val, grid[i]->e, 1)) && // Si il n'y a aucun problème
        (!(check_colunm(grid, grid[i]->val, grid[i]->n, -1) || check_colunm(grid, grid[i]->val, grid[i]->s, 1))) &&
        (!check_zone(grid, grid[i]->val, grid[i])))) {
            i++; // On passe a la case suivante
            printf("Case suivante : coor %d-%d\n", get_posx(grid[i]), get_posy(grid[i]));
            print_grid(grid);
        }
    }
}

void print_grid(case_t* grid[81]) {
    {
    for (int i = 0; i < 81; i += 9)
    {
        case_t *cas = grid[i];

        while (cas)
        { // itérer vers l'est jusqu'à ce que nous n'obtenions pas null
            if (cas->val == 0)
            {
                printf("- ");
            }
            else
            {
                printf("%d ", cas->val);
            }
            cas = cas->e;
        }

        
        printf("\n");
    }
}

}

int check_line(case_t* grid[81], int x, case_t* cas, int direction) {
     if (cas == NULL) // si nous avions atteint le bord, le cas deviendra NULL
    {
        return 0;
    }
    else if (cas->val == x) // si la valeur en cas devient égale à la valeur x que nous essayons de rechercher
    {
        return 1;
    }

    if (direction == -1)
    {                                                  // Haut
        return check_line(grid, x, cas->w, direction); // Maintenant, nous devons appeler la même fonction avec le pointeur ouest pour explorer le côté gauche
    }
    else
    {                                                  // Bas
        return check_line(grid, x, cas->e, direction); // Maintenant, nous devons appeler la même fonction pour le pointeur est si la position est 1
    }

}

int check_colunm(case_t* grid[81], int x, case_t* cas, int direction) {
        if (cas == NULL)
    {
        return 0;
    }
    else if (cas->val == x)
    {
        return 1;
    }

    if (direction == -1)
    { // Haut
        return check_colunm(grid, x, cas->n, direction);
    }
    else
    { // Bas
        return check_colunm(grid, x, cas->s, direction);
    }

}

int check_zone(case_t* grid[81], int x, case_t* cas) {
    int nmb = 0;
    case_t* tmp = get_middle(cas, grid);
    nmb += tmp->nw->val == x;
    nmb += tmp->n->val == x;
    nmb += tmp->ne->val == x;
    nmb += tmp->w->val == x;
    nmb += tmp->val == x;
    nmb += tmp->e->val == x;
    nmb += tmp->sw->val == x;
    nmb += tmp->s->val == x;
    nmb += tmp->se->val == x;
    if (nmb == 1)
        return 0;
    else
        return 1;
}

int get_posx(case_t* cas) {

    if (cas->w == NULL)
    {
        return 0;
    }

    return get_posx(cas->w) + 1;
}

int get_posy(case_t* cas) {
     if (cas->n == NULL)
    { // si cas est nul, nous devons retourner 0 car nous n'allons pas explorer plus avant
        return 0;
    }
    return get_posy(cas->n) + 1; // À chaque retour, nous devons retourner plus une valeur

}

case_t* get_middle(case_t* cas, case_t* grid[81]) {
    int x = (get_posx(cas)/3)*3+1, y = (get_posy(cas)/3)*3+1;
    return grid[x+y*9];
}


int main(int argc, char ** argv) {
    // mettre en argument le nom du fichier de la grille sudoku a résoudre,
    // par default, si on met rien le nom du fichier pris en compte sera "sudoku"
    case_t* grid[81];
    if (argc == 1)
        read_grid("sudoku.txt", grid);
    else
        read_grid(argv[argc], grid);
    printf("Grille :\n");
    print_grid(grid);
    solve(grid);
    printf("\nSolution :\n");
    print_grid(grid);
    free_grid(grid);
    return 0;
}