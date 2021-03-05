#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define lambda 0.05
#define ITERATIONS 1000

float A[4][5]=
    {{1,1,1,1,1},
     {1,0,0,0,1},
     {1,1,1,1,1},
     {1,0,0,0,1}
};
float C[4][5]=
    {{1,1,1,1,1},
     {1,0,0,0,0},
     {1,0,0,0,0},
     {1,1,1,1,1}
};

float U[4][5]=
        {{0,0,0,0,0},
         {0,0,0,0,0},
         {0,0,0,0,0},
         {0,0,0,0,0}
        };

float weightsA[4][5]=
    {{0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,0,0,0},
     {0,0,0,0,0}
};

float weightsC[4][5]=
        {{0,0,0,0,0},
         {0,0,0,0,0},
         {0,0,0,0,0},
         {0,0,0,0,0}
};

double erreurA[ITERATIONS];
double erreurC[ITERATIONS];
double iterations[ITERATIONS];

void calculU(){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            U[i][j] = rand()%1;
        }
    }
}


void affichage(float tableau[4][5]){
    for (int i = 0; i<4; i++){
        printf("\n");
        for (int j = 0; j<5; j++){
            printf("%f\t",tableau[i][j]);
        }
    }
    printf("\n");
}

void affichageCourbes(double xvals[ITERATIONS], double yvals[ITERATIONS] ) {
    char *commandsForGnuplot[] = {"set title \"TITLE\"", "plot 'data.temp'"};
    FILE * gnuplotPipe = popen ("gnuplot -p", "w");
    fprintf(gnuplotPipe,"set style fill transparent solid 0.5\n");
    fprintf(gnuplotPipe, "plot '-' w linespoints\n");
    int i;
    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(gnuplotPipe, "%lf %lf\n", xvals[i], yvals[i]);
    }
    fprintf(gnuplotPipe, "e");
}

int randomizer(float entree[4][5], int boucle){
    for (int i=0; i<boucle; i++) {
        int randomValue = rand() % 1;
        int randomLine = rand() % 4;
        int randomColumn = rand() % 5;
        entree[randomLine][randomColumn] = (float) randomValue;
    }
}

void calculA(float tableau[4][5],float value, int nombre) {
    printf("L'entrée est un %f\n", value);
    float entree[4][5];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            entree[i][j] = tableau[i][j];
        }
    }
    //randomizer(entree,5);
    double somme = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++){
            somme += entree[i][j] * weightsA[i][j];
        }
    }
    somme = 1/(1+exp(-somme));
    printf("L'erreur est de: %f\n",(value - somme));
    erreurA[nombre] = sqrt(pow(value-somme,2));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            weightsA[i][j] = weightsA[i][j] + lambda * (value - somme) * entree[i][j];
        }
    }
    printf("J'ai détecté un %f \n\n", somme);
}

void calculC(float tableau[4][5],float value, int nombre) {
    printf("L'entrée est un %f\n", value);
    float entree[4][5];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            entree[i][j] = tableau[i][j];
        }
    }
    //randomizer(entree,5);
    double somme = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++){
            somme += entree[i][j] * weightsC[i][j];
        }
    }
    somme = 1/(1+exp(-somme));
    printf("L'erreur est de: %f\n",(value - somme));
    erreurC[nombre] = sqrt(pow(value-somme,2));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            weightsC[i][j] = weightsC[i][j] + lambda * (value - somme) * entree[i][j];
        }
    }
    printf("J'ai détecté un %f \n\n", somme);
}

int main() {
    srand(time(NULL));
    calculU();
    affichage(U);
    int i = 0;
    while (i < ITERATIONS){
        int random = rand()%3;
        if (random % 3 == 0) {
            calculA(A,1,i);
            calculC(A,0,i);
        }
        else if (random % 3 == 1){
            calculA(C,0,i);
            calculC(C,1,i);}
        else{
            calculA(U,0,i);
            calculC(U,0,i);
        }
        iterations[i] = i;
        i++;
    }
    affichageCourbes(iterations, erreurA);
    affichageCourbes(iterations, erreurC);

}