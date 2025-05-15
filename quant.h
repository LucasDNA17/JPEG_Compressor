#ifndef QUANT_H
    #define QUANT_H

    #include "matrix.h"

    void quantizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);
    void quantizacao_inversa(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);
    void divide_bloco(double **bloco, double divisor[8][8]);

#endif