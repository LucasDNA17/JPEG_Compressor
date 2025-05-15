#include "matrix.h"

void desaloca_matrix(void **matrix, int Height, int Width) {
    if(matrix == NULL)
        return;

    for(int i = 0; i < Height;i++)
        for(int j = 0; j < Width; j++) {
            free(matrix[i]);
            matrix[i] = NULL;
        }

    free(matrix);
    matrix = NULL;
}

//0 - unsigned char
//1 - double
void **aloca_matrix(int tipo, int Height, int Width) {
    void **matrix = (void **) malloc(Height*sizeof(void *));
    if(matrix == NULL) {
        printf("Erro na alocação de matrix!\n");
        return NULL;
    }
    
    for(int i = 0; i < Height; i++) {
        if(tipo == 0) {
            matrix[i] = (unsigned char *) malloc(Width*sizeof(unsigned char));
        }
        else if(tipo == 1)
            matrix[i] = (double *) malloc(Width*sizeof(double));
    }

    return matrix;
}


/*
void multiplicacao_matrix(double matrix1[8][8], double matrix2[8][8], double resultado[8][8]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            resultado[i][j] = 0;
            for(int k = 0; k < 8; k++)
                resultado[i][j] += matrix1[i][k]*matrix2[k][j];
        }
    }
}
*/


double **multiplicacao_matrix_esq(double **matrix1,  double matrix2[8][8]) {
    double **resultado = (double **) aloca_matrix(1, 8, 8);
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            resultado[i][j] = 0;
            for(int k = 0; k < 8; k++)
                resultado[i][j] += matrix1[i][k]*matrix2[k][j];
        }
    }

    return resultado;
}


double **multiplicacao_matrix_dir(double matrix1[8][8], double **matrix2) {
    double **resultado = (double **) aloca_matrix(1, 8, 8);
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            resultado[i][j] = 0;
            for(int k = 0; k < 8; k++)
                resultado[i][j] += matrix1[i][k]*matrix2[k][j];
        }
    }
    return resultado;
}


void get_bloco_matrix(double **matrix, double bloco[8][8], int bases[2]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++)
            bloco[i][j] = matrix[bases[0] + i][bases[1] + j];
    }
}

void set_bloco_matrix(double **matrix, double bloco[8][8], int bases[2]) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++)
            matrix[bases[0] + i][bases[1] + j] = bloco[i][j];
    }
}


void printaMatrix(double **matrix, int Height, int Width) {
    if(matrix == NULL)
        return;


    for(int i = 0; i < Height; i++) {
        for(int j = 0; j < Width; j++)
            printf("%lf ", matrix[i][j]);

        printf("\n");
    }
}