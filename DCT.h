#ifndef DCT_H
    #define DCT_H

    #include "matrix.h"
    #include "BMP_ColorElements.h"


    /*
    void DCT(Imagem_ycbcr *imagem, int *indices_y, int *indices_c);
    void DCT_inversa(Imagem_ycbcr *imagem,int *indices_y, int *indices_c);
    void DCT_8x8(double bloco[8][8], double resultado[8][8]);
    void DCT_inversa8x8(double bloco[8][8], double resultado[8][8]);
    */

    double **DCT8x8(double **matrix);
    void DCT (double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);
    void DCT_inversa(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);

#endif