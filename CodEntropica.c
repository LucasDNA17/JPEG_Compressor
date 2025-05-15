#include "CodEntropica.h"



 int *vetorizacao_bloco8x8(double **bloco8x8) {
    if(bloco8x8 == NULL) return NULL;

    int index_matrix[8][8] = {
    {0, 1, 5 ,6, 14, 15, 27, 28},
    {2, 4, 7, 13, 16, 26, 29, 42},
    {3, 8, 12, 17, 25, 30, 41, 43},
    {9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
    };

    int *vetor = (int *) malloc(64*sizeof(int));
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            vetor[index_matrix[i][j]] = (int) bloco8x8[i][j];
    
    
    desaloca_matrix((void **) bloco8x8, 8, 8);
    return vetor;    
}


double **matrizicacao_bloco8x8(int *vetor) {
    if(vetor == NULL) return NULL;

    int index_matrix[8][8] = {
    {0, 1, 5 ,6, 14, 15, 27, 28},
    {2, 4, 7, 13, 16, 26, 29, 42},
    {3, 8, 12, 17, 25, 30, 41, 43},
    {9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
    };

    double **bloco8x8 = (double **) aloca_matrix(1, 8, 8);
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            bloco8x8[i][j] = (double) vetor[index_matrix[i][j]]; 
            


    free(vetor);
    return bloco8x8;    
}


int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos8x8 == NULL) return NULL;

    int ***vetores = (int ***) malloc(3*sizeof(int **));
    vetores[0] = (int **) malloc(qtd_blocos_y*sizeof(int *));
    vetores[1] = (int **) malloc(qtd_blocos_c*sizeof(int *));
    vetores[2] = (int **) malloc(qtd_blocos_c*sizeof(int *));

    for(int i = 0; i < qtd_blocos_y; i++)
        vetores[0][i] = vetorizacao_bloco8x8(blocos8x8[0][i]);

    for(int i = 0; i < qtd_blocos_c; i++) {
        vetores[1][i] = vetorizacao_bloco8x8(blocos8x8[1][i]);
        vetores[2][i] = vetorizacao_bloco8x8(blocos8x8[2][i]);
    }

    free(blocos8x8[0]); free(blocos8x8[1]); free(blocos8x8[2]);
    free(blocos8x8);

    return vetores;
}


double ****matrizicacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c) {
    if(vetor == NULL) return NULL;

    double ****blocos8x8 = (double ****) malloc(3*sizeof(double ***));

    blocos8x8[0] = (double ***) malloc(qtd_blocos_y*sizeof(double **));
    blocos8x8[1] = (double ***) malloc(qtd_blocos_c*sizeof(double **));
    blocos8x8[2] = (double ***) malloc(qtd_blocos_c*sizeof(double **));

    for(int i = 0; i < qtd_blocos_y; i++)
        blocos8x8[0][i] = matrizicacao_bloco8x8(vetor[0][i]);

    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos8x8[1][i] = matrizicacao_bloco8x8(vetor[1][i]);
        blocos8x8[2][i] = matrizicacao_bloco8x8(vetor[2][i]);
    }

    free(vetor[0]); free(vetor[1]); free(vetor[2]);
    free(vetor);

    return blocos8x8;
}



