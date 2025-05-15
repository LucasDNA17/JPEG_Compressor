#ifndef COD_ENTROPICA_H
    #define COD_ENTROPICA_H

    #include "matrix.h"

    int *vetorizacao_bloco8x8(double **bloco8x8);
    double **matrizicacao_bloco8x8(int *vetor);
    int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);
    double ****matrizicacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c);

#endif