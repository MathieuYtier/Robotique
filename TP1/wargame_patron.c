/************ Ce TPs fut construit par Philippe Gaussier dans les années 90, il est repris et modifié pour les enseignements de Nils Beaussé ************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define NB_LIGNES 5
#define NB_COLONNES 5
#define INFINI 10000
#define PROFONDEUR 4



/* pour simplifier la saisie par rapport a l'ordre des variables
([i][j]: i abscisse, j ordonnee */
/*int jeu_init_transpose[TAILLE][TAILLE]=
{
     {-1,-2,-2,-1, 0 },
     { 0,-1,-1, 0, 0 },
     { 0, 0, 0, 0, 0 },
     { 0, 1, 1, 0, 0 },
     { 1, 2, 2, 1, 0 }
};*/

//Tableau de test avec un seul pion par joueur, une fois testé votre algorithme, il faut plutot utiliser celui du dessus, qui représente un plateau complet tel que vu dans le sujet de TP
int jeu_init_transpose[NB_LIGNES][NB_COLONNES] =
        {
               /*{ 0, 0,-2, 0, 0 },
                { 0, 0,-1, 0, 0 },
                { 0, 0, 0, 0, 0 },
                { 0, 3, 0, 0, 0 },
                { 0, 0, 0, 0, 0 }*/
                {-1,-2,-2,-1, 0 },
                { 0,-1,-1, 0, 0 },
                { 0, 0, 0, 0, 0 },
                { 0, 1, 1, 0, 0 },
                { 1, 2, 2, 1, 0 }
        };


// Pour avoir des printf automatique de debug, il suffit de decommenter la ligne suivante
/*#define DEBUG*/

// La structure pour chaque pion, chaque pion est defini par sa couleur (joueur à qui il appartient), et sa valeur
typedef struct pion_s {
    int couleur;
    int valeur;
} Pion;

// le pointeur vers le début de la zone mémoire du plateau de jeu. C'est un plateau de pion, le début du tableau est donc une case de type "pion", le pointeur est donc un pointeur de pion, type pion*
Pion *plateauDeJeu;

// En-tête de fonctions
void f_affiche_plateau(Pion *plateau);

int f_convert_char2int(char c);

char f_convert_int2char(int i);


// Convertit les caracteres en int. Méthode simple.
int f_convert_char2int(char c) {
    if (c >= 'A' && c <= 'Z')
        return (int) (c - 'A');
    if (c >= 'a' && c <= 'z')
        return (int) (c - 'a');
    return -1;
}

// Fonction inverse de la précédente
char f_convert_int2char(int i) {
    return (char) i + 'A';
}

// Initialise le tableau de type pion avec le tableau de int "jeu_init_transpose" defini tout en haut, on sépare notemment la valeur de l'appartenance, alors que le tableau jeu_init_transpose combine les deux pour que ce soit plus simple à visualiser et modifier
Pion *f_init_plateau() {
    int i, j;
    Pion *plateau = NULL;
    plateau = (Pion *) malloc(NB_LIGNES * NB_COLONNES * sizeof(Pion));
    if (plateau == NULL) {
        printf("error: unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NB_LIGNES; i++) {
        for (j = 0; j < NB_COLONNES; j++) {
            //On associe le plateau alloué precedemment, de type "pion" au tableau d'initialisation qu'on a definis tout en haut de façon "visuelle"

            if (jeu_init_transpose[i][j] == 0) {
                //Les pions du jeu qui ont une valeur de 0 n'appartiennent à personne, leur couleur est donc 0.
                plateau[i * NB_COLONNES + j].valeur = 0;
                plateau[i * NB_COLONNES + j].couleur = 0;
            }
            // Si le chiffre dand jeu_init_transpose est negatif...
            if (jeu_init_transpose[i][j] < 0) {
                //alors on le renormalise, car "valeur" dans la structure pion est un chiffre indiquant la valeur de la piece (entre 0 et 3) et non son appartenance
                plateau[i * NB_COLONNES + j].valeur = -jeu_init_transpose[i][j];
                // Et on attribue à "couleur" la valeur du signe, qui indique le joueur à qui appartient le pion (ici -1)
                plateau[i * NB_COLONNES + j].couleur = -1;
            }
            if (jeu_init_transpose[i][j] > 0) {
                // Dans le cas contraire on a pas a modifier la valeur qui est déja positive, donc on la copie
                plateau[i * NB_COLONNES + j].valeur = jeu_init_transpose[i][j];
                // et on indique que la couleur est bien celle de l'autre joueur. (ici 1)
                plateau[i * NB_COLONNES + j].couleur = 1;
            }
        }
    }

    return plateau;
}

// Affiche le plateau passé en argument sur le terminal sous une forme (à peu prés) lisible. 
void f_affiche_plateau(Pion *plateau) {
    int i, j, k;

    printf("\n    ");
    for (k = 0; k < NB_COLONNES; k++)
        printf("%2c ", f_convert_int2char(k));
    printf("\n    ");
    for (k = 0; k < NB_COLONNES; k++)
        printf("-- ");
    printf("\n");
    for (i = NB_LIGNES - 1; i >= 0; i--) {
        printf("%2d ", i);
        for (j = 0; j < NB_COLONNES; j++) {
            printf("|");
            switch (plateau[i * NB_COLONNES + j].couleur) {
                case -1:
                    printf("%do", plateau[i * NB_COLONNES + j].valeur);
                    break;
                case 1:
                    printf("%dx", plateau[i * NB_COLONNES + j].valeur);
                    break;
                default:
                    printf("  ");
            }
        }
        printf("|\n    ");
        for (k = 0; k < NB_COLONNES; k++)
            printf("-- ");
        printf("\n");
    }
    printf("    ");
}

// Cette fonction permet de verifier si l'un des joueurs verifie les conditions de victoire. Atteint-il la derniere ligne ? ou a t-il pris tout les pions de l'adversaire ? La fonction retourne le no du joueur gagnant, ou 0.
int f_gagnant(Pion *plateau) {
    int i, j, somme1 = 0, somme2 = 0;

    //Quelqu'un est-il arrive sur la ligne de l'autre ? Verification ->
    for (i = 0; i < NB_COLONNES; i++) {
        if (plateau[i].couleur == 1)
            return 1;
        if (plateau[(NB_LIGNES - 1) * NB_COLONNES + i].couleur == -1)
            return -1;
    }

    //taille des armees
    for (i = 0; i < NB_LIGNES; i++) {
        for (j = 0; j < NB_COLONNES; j++) {
            if (plateau[i * NB_COLONNES + j].couleur == 1)
                somme1++;
            if (plateau[i * NB_COLONNES + j].couleur == -1)
                somme2++;
        }
    }
    if (somme1 == 0)
        return -1;
    if (somme2 == 0)
        return 1;

    return 0;
}


/**
 * Prend comme argument la ligne et la colonne de la case
 * 	pour laquelle la bataille a lieu
 * Renvoie le couleur du gagnant
 * */
int f_bataille(int l, int c) {
    int i, j, mini, maxi, minj, maxj;
    int somme = 0;

    mini = l - 1 < 0 ? 0 : l - 1;
    maxi = l + 1 > NB_LIGNES - 1 ? NB_LIGNES - 1 : l + 1;
    minj = c - 1 < 0 ? 0 : c - 1;
    maxj = c + 1 > NB_COLONNES - 1 ? NB_COLONNES - 1 : c + 1;

    for (i = mini; i <= maxi; i++) {
        for (j = minj; j <= maxj; j++) {
            somme += plateauDeJeu[i * NB_COLONNES + j].couleur * plateauDeJeu[i * NB_COLONNES + j].valeur;
        }
    }
    somme -= plateauDeJeu[l * NB_COLONNES + c].couleur * plateauDeJeu[l * NB_COLONNES + c].valeur;

    if (somme < 0)
        return -1;
    if (somme > 0)
        return 1;

    return plateauDeJeu[l * NB_COLONNES + c].couleur;
}


/**
 * Verifie la validité d'un mouvement ! 
 * Prend la ligne et colonne de la case d'origine
 * 	et la ligne et colonne de la case de destination
 * Renvoie 1 en cas d'erreur
 * Renvoie 0 sinon
 * */
int f_test_mouvement(Pion *plateau, int l1, int c1, int l2, int c2, int couleur) {
    /* Erreur, hors du plateau */
    if (l1 < 0 || l1 >= NB_LIGNES || l2 < 0 || l2 >= NB_LIGNES ||
        c1 < 0 || c1 >= NB_COLONNES || c2 < 0 || c2 >= NB_COLONNES)
        return 1;
    /* Erreur, il n'y a pas de pion a deplacer ou le pion n'appartient pas au joueur*/
    if (plateau[l1 * NB_COLONNES + c1].valeur == 0 || plateau[l1 * NB_COLONNES + c1].couleur != couleur)
        return 1;
    /* Erreur, tentative de tir fratricide */
    if (plateau[l2 * NB_COLONNES + c2].couleur == plateau[l1 * NB_COLONNES + c1].couleur)
        return 1;
    /* Erreur, on tente de se déplacer de plus d'une case ou de 0 case !!! */
    if (l1 - l2 > 1 || l2 - l1 > 1 || c1 - c2 > 1 || c2 - c1 > 1 || (l1 == l2 && c1 == c2))
        return 1;

    return 0;
}


/**
 * Prend la ligne et colonne de la case d'origine
 * 	et la ligne et colonne de la case de destination
 *  et effectue le traitement de l'operation demandée
 * Renvoie 1 en cas d'erreur
 * Renvoie 0 sinon
 * */
int f_bouge_piece(Pion *plateau, int l1, int c1, int l2, int c2, int couleur) {
    int gagnant = 0;

    if (f_test_mouvement(plateau, l1, c1, l2, c2, couleur) != 0)
        return 1;

    /* Cas ou il n'y a personne a l'arrivee */
    if (plateau[l2 * NB_COLONNES + c2].valeur == 0) {
        plateau[l2 * NB_COLONNES + c2].couleur = plateau[l1 * NB_COLONNES + c1].couleur;
        plateau[l2 * NB_COLONNES + c2].valeur = plateau[l1 * NB_COLONNES + c1].valeur;
        plateau[l1 * NB_COLONNES + c1].couleur = 0;
        plateau[l1 * NB_COLONNES + c1].valeur = 0;
    } else {
        gagnant = f_bataille(l2, c2);
        /* victoire : on écrase le pion visé */
        if (gagnant == couleur) {
            plateau[l2 * NB_COLONNES + c2].couleur = plateau[l1 * NB_COLONNES + c1].couleur;
            plateau[l2 * NB_COLONNES + c2].valeur = plateau[l1 * NB_COLONNES + c1].valeur;
            plateau[l1 * NB_COLONNES + c1].couleur = 0;
            plateau[l1 * NB_COLONNES + c1].valeur = 0;
        }
            /* defaite : on disparait */
        else if (gagnant != 0) {
            plateau[l1 * NB_COLONNES + c1].couleur = 0;
            plateau[l1 * NB_COLONNES + c1].valeur = 0;
        }
    }

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
    return 0;
}

//Calcul du nombre de pions sur le plateau du joueur
int f_nbPions(Pion *jeu, int joueur) {
    int nbPion = 0;
    int i, j;
    for (i = 0; i < NB_COLONNES; ++i) {
        for (j = 0; j < NB_LIGNES; ++j) {
            if (jeu[i * NB_COLONNES + j].couleur == joueur) {
                ++nbPion;
            }
        }
    }
    return nbPion;
}

//Calcul de la somme de la valeur de tous les pions du joueur
int f_valeur(Pion *jeu, int joueur) {
    int i, j;
    int valeur = 0;
    for (i = 0; i < NB_LIGNES; ++i) {
        for (j = 0; j < NB_COLONNES; ++j) {
            if (jeu[i * NB_COLONNES + j].couleur == joueur) {
                valeur += jeu[i * NB_COLONNES + j].valeur;
            }
        }
    }
    return valeur;
}

//fonction d'évaluation
int f_eval(Pion *jeu, int joueur) {
    /** A remplir par l'étudiant **/
    /** A vous de trouver une fonction efficace **/
}

//copie du plateau
void f_copie_plateau(Pion *source, Pion *destination) {
    int i, j;
    for (i = 0; i < NB_LIGNES; i++) {
        for (j = 0; j < NB_COLONNES; j++) {
            destination[i * NB_COLONNES + j].couleur = source[i * NB_COLONNES + j].couleur;
            destination[i * NB_COLONNES + j].valeur = source[i * NB_COLONNES + j].valeur;
        }
    }
}

//mise a zero du plateau
Pion *f_raz_plateau() {
    Pion *jeu = NULL;
    int i, j;
    jeu = (Pion *) malloc(NB_LIGNES * NB_COLONNES * sizeof(Pion));
    for (i = 0; i < NB_LIGNES; i++) {
        for (j = 0; j < NB_COLONNES; j++) {
            jeu[i * NB_COLONNES + j].couleur = 0;
            jeu[i * NB_COLONNES + j].valeur = 0;
        }
    }
    return jeu;
}

//Fonction min trouve le minimum des noeuds fils
//??? f_min(???)
//		{
/** A remplir par l'étudiant **/
//		}

//Fonction max trouve le maximum des noeuds fils
//??? f_max(???)
//		{
/** A remplir par l'étudiant **/
//		}



int f_heuristique(Pion *plateau, int joueur) {

    return f_valeur(plateau, joueur) - f_valeur(plateau, joueur * -1);
}

/** Calcule et joue le meilleur cout **/
int f_IA(int joueur, int profondeur, Pion *plateau, int joueurInitial, int* movLmax, int* movCmax, int*line, int*column) {
    int a,b,c,d;
    //f_affiche_plateau(plateau);
    int vmax = 0, vmax_calc = -100000, vtemp = 0;
    if (profondeur != 0 && f_gagnant(plateau) == 0) {
        for (int ligne = 0; ligne < NB_LIGNES; ligne++) {
            for (int colonne = 0; colonne < NB_COLONNES; colonne++) {
                if (plateau[ligne * NB_COLONNES + colonne].couleur == joueur) {
                    for (int movL = -1; movL < 2; movL++) {
                        for (int movC = -1; movC < 2; movC++) {
                            Pion *plateauCopie;
                            plateauCopie = f_init_plateau();
                            f_copie_plateau(plateau, plateauCopie);
                            if (f_test_mouvement(plateauCopie, ligne, colonne, ligne + movL, colonne + movC, joueur) ==
                                0) {
                                f_bouge_piece(plateauCopie, ligne, colonne, ligne + movL, colonne + movC, joueur);
                                vtemp = joueurInitial * joueur * f_IA(-joueur, profondeur - 1, plateauCopie, joueurInitial,&a,&b,&c,&d);
                                if (vtemp > vmax_calc) {
                                    vmax_calc = vtemp;
                                    *movLmax = movL;
                                    *movCmax = movC;
                                    *line = ligne;
                                    *column = colonne;
                                }
                                free(plateauCopie);

                            }
                        }
                    }
                }
            }
        }
        vmax = joueurInitial * joueur * vmax_calc;
    } else if (profondeur == 0 && f_gagnant(plateau) == 0) {
        vmax = f_heuristique(plateau, joueurInitial);
    } else {
        if (f_gagnant(plateau) == joueurInitial) {
            vmax = 10000;
        } else {
            vmax = -10000;
        }
    }
    /*printf("La profondeur est de: %i\n", profondeur);
    printf("La valeur max de ce tableau est de : %i \n", vmax);
    printf("\t----------------------------------\n");*/
    return vmax;
}


/**
 * Demande le choix du joueur humain et calcule le coup demande
 * */
void f_humain(int joueur) {
    char c1, c2;
    char buffer[32];
    int l1, l2;
    int test;
    printf("joueur ");
    switch (joueur) {
        case -1:
            printf("o ");
            break;
        case 1:
            printf("x ");
            break;
        default:
            printf("inconnu ");
    }
    printf("joue:\n");
    while (1) {
        fflush(stdin);
        fgets(buffer, 32, stdin);
        if (sscanf(buffer, "%c%i%c%i\n", &c1, &l1, &c2, &l2) == 4) {
            if (f_bouge_piece(plateauDeJeu, l1, f_convert_char2int(c1), l2, f_convert_char2int(c2), joueur) == 0)
                break;
        }
        fflush(stdin);
        printf("mauvais choix\n");
    }
}

// La boucle du jeu-vidéo
int main(int argv, char *argc[]) {
    int fin = 0, mode = 0, ret, joueur = 1;
    int movl, l, movc,c;
    printf("1 humain vs IA\n2 humain vs humain\n3 IA vs IA\n");
    scanf("%d", &mode);

    plateauDeJeu = f_init_plateau();
    while (!fin) {
        printf("Voici le plateau de jeu: \n");
        f_affiche_plateau(plateauDeJeu);
        if (mode == 1) {
            if (joueur > 0)
                f_humain(joueur);
            else {
                f_IA(joueur, PROFONDEUR, plateauDeJeu, joueur, &movl, &movc, &l, &c);
                f_bouge_piece(plateauDeJeu, l, c, l + movl, c + movc, joueur);
            }
        } else if (mode == 2) {
            f_humain(joueur);
        } else {
            f_IA(joueur, PROFONDEUR, plateauDeJeu, joueur, &movl, &movc, &l, &c);
            f_bouge_piece(plateauDeJeu, l, c, l + movl, c + movc, joueur);
        }

        if ((ret = f_gagnant(plateauDeJeu)) != 0) {
            switch (ret) {
                case 1:
                    printf("Voici le plateau de jeu: \n");
                    f_affiche_plateau(plateauDeJeu);
                    printf("joueur x gagne!\n");
                    fin = 1;
                    break;
                case -1:
                    printf("Voici le plateau de jeu: \n");
                    f_affiche_plateau(plateauDeJeu);
                    printf("joueur o gagne!\n");
                    fin = 1;
                    break;
            }
        }
        joueur = -joueur;
    }

    return 0;
}

