#include "DCT.h"
#include "BMP_ColorElements.h"


//Matriz pré-calculada da DCT aplicada a um bloco 8x8.
double matriz_DCT[8][8] = {
    {0.353553, 0.353553, 0.353553, 0.353553, 0.353553, 0.353553, 0.353553, 0.353553},
    {0.490393, 0.415735, 0.277785, 0.097545, -0.097545, -0.277785, -0.415735, -0.490393},
    {0.461940, 0.191342, -0.191342, -0.461940, -0.461940, -0.191342, 0.191342, 0.461940},
    {0.415735, -0.097545, -0.490393, -0.277785, 0.277785, 0.490393, 0.097545, -0.415735},
    {0.353553, -0.353553, -0.353553, 0.353553, 0.353553, -0.353553, -0.353553, 0.353553},
    {0.277785, -0.490393, 0.097545, 0.415735, -0.415735, -0.097545, 0.490393, -0.277785},
    {0.191342, -0.461940, 0.461940, -0.191342, -0.191342, 0.461940, -0.461940, 0.191342},
    {0.097545, -0.277785, 0.415735, -0.490393, 0.490393, -0.415735, 0.277785, -0.097545}
};

//Transposta da matriz DCT.
double matriz_DCT_transposta[8][8] = {
    {0.353553,  0.490393,  0.461940,  0.415735,  0.353553,  0.277785,  0.191342,  0.097545},
    {0.353553,  0.415735,  0.191342, -0.097545, -0.353553, -0.490393, -0.461940, -0.277785},
    {0.353553,  0.277785, -0.191342, -0.490393, -0.353553,  0.097545,  0.461940,  0.415735},
    {0.353553,  0.097545, -0.461940, -0.277785,  0.353553,  0.415735, -0.191342, -0.490393},
    {0.353553, -0.097545, -0.461940,  0.277785,  0.353553, -0.415735, -0.191342,  0.490393},
    {0.353553, -0.277785, -0.191342,  0.490393, -0.353553, -0.097545,  0.461940, -0.415735},
    {0.353553, -0.415735,  0.191342,  0.097545, -0.353553,  0.490393, -0.461940,  0.277785},
    {0.353553, -0.490393,  0.461940, -0.415735,  0.353553, -0.277785,  0.191342, -0.097545}
};


/* ----- Funções auxiliares ----- */


//Função que aplica a DCT a um bloco 8x8. A DCT é aplicada na forma de uma multiplicação de matrizes:
//  - DCT(Bloco8x8) = C x Bloco8x8 x C_transposta, onde C é a matriz pré-calculada da DCT.
//Entrada: ponteiro duplo para double (bloco 8x8).
//Saída: ponteiro duplo para double, que é a matriz 8x8 resultado da DCT.
double **DCT8x8(double **matrix) {
    //Faz C x Bloco8x8.
    double **resultado_parcial = multiplicacao_matrix_dir(matriz_DCT, matrix);
    //Faz (C x Bloco8x8) x (C_transposta)
    double **DCT8x8 = multiplicacao_matrix_esq(resultado_parcial, matriz_DCT_transposta);
    //Desaloca o resultado parcial da primeira operação.
    desaloca_matrix((void **) resultado_parcial, 8, 8);

    return DCT8x8;
}

//Função que aplica a DCT inversa a um bloco 8x8. Como a matriz C da DCT é ortogonal, a DCT inversa
//é aplicada na forma de uma multiplicação de matrizes:
//  - DCT_inversa(Bloco8x8) = C_transposta x Bloco8x8 x C, onde C é a matriz pré-calculada da DCT.
//Entrada: ponteiro duplo para double (bloco 8x8).
//Saída: ponteiro duplo para double, que é a matriz 8x8 resultado da DCT inversa.
double **DCT_inversa8x8(double **matrix) {
    //Faz C_transposta x Bloco8x8.
    double **resultado_parcial = multiplicacao_matrix_dir(matriz_DCT_transposta, matrix);
    //Faz (C_transposta x Bloco8x8) x C
    double **DCT8x8 = multiplicacao_matrix_esq(resultado_parcial, matriz_DCT);
    //Desaloca o resultado parcial da primeira operação.
    desaloca_matrix((void **) resultado_parcial, 8, 8);

    return DCT8x8;
}


/* ----- Funções principais ----- */


//Função que realiza a DCT nos blocos 8x8 de uma imagem.
//Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
//da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
//indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos nos canais de crominância
//e luminância.
//Saída: nenhuma.
void DCT (double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    //Aplica a DCT nos blocos do canal Y.
    for(int i = 0; i < qtd_blocos_y; i++) {
        double **aux = blocos8x8[0][i];
        blocos8x8[0][i] = DCT8x8(blocos8x8[0][i]);
        desaloca_matrix((void **) aux, 8, 8);
    }

    //Aplica a DCT nos blocos dos canais Cb e Cr.
    for(int i = 0; i < qtd_blocos_c; i++) {
        double **aux_cb = blocos8x8[1][i];
        double **aux_cr = blocos8x8[2][i];
        blocos8x8[1][i] = DCT8x8(blocos8x8[1][i]);
        blocos8x8[2][i] = DCT8x8(blocos8x8[2][i]);
        desaloca_matrix((void **) aux_cb, 8, 8);
        desaloca_matrix((void **) aux_cr, 8, 8);
    }

}

//Função que realiza a DCT inversa nos blocos 8x8 de uma imagem.
//Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
//da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
//indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos nos canais de crominância
//e luminância.
//Saída: nenhuma.
void DCT_inversa(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    //Aplica a DCT inversa nos blocos do canal Y
    for(int i = 0; i < qtd_blocos_y; i++) {
        double **aux = blocos8x8[0][i];
        blocos8x8[0][i] = DCT_inversa8x8(blocos8x8[0][i]);
        desaloca_matrix((void **) aux, 8, 8);
    }

    //Aplica a DCT inversa nos blocos dos canais Cb e Cr.
    for(int i = 0; i < qtd_blocos_c; i++) {
        double **aux_cb = blocos8x8[1][i];
        double **aux_cr = blocos8x8[2][i];
        blocos8x8[1][i] = DCT_inversa8x8(blocos8x8[1][i]);
        blocos8x8[2][i] = DCT_inversa8x8(blocos8x8[2][i]);
        desaloca_matrix((void **) aux_cb, 8, 8);
        desaloca_matrix((void **) aux_cr, 8, 8);
    }

}













