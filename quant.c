#include "quant.h"


//Como as tabelas de quantização do compressor são fixas,
//elas não são salvas no arquivo comprimido e são usadas
//diretamente no código.

//Tabela para quantização do canal de luminância 
const double quantizacao_luminancia[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {79, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

//Tabela para quantização do canal de crominância
const double quantizacao_crominancia[8][8] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}
};


/* ----- Funções auxiliares ----- */


//Função que divide todos os elementos de um bloco pelos elementos
//correspondentes de uma matriz.
//Entrada: matriz 8x8 (bloco) e a matriz 8x8 que contém os divisores.
//Saída: nenhuma
void divide_bloco(double **bloco, const double divisor[8][8]) {
    if(bloco == NULL) return;
    
    //Divide os elementos do bloco pelo elemento da matriz e pelo
    //fator de compressão, arredondando para o inteiro mais próximoS.
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++)
            bloco[i][j] = round(bloco[i][j]/(divisor[i][j]*fator_compressao));
    }
}


//Função que multiplica todos os elementos de um bloco pelos elementos
//correspondentes de uma matriz.
//Entrada: matriz 8x8 (bloco) e a matriz 8x8 que contém os valores que multiplicam.
//Saída: nenhuma
void multiplica_bloco(double **bloco, const double multiplicador[8][8]) {
    if(bloco == NULL) return;
    
    //Multiplica os elementos do bloco pelo elemento da matriz e pelo
    //fator de compressão.
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++)
            bloco[i][j] = bloco[i][j]*multiplicador[i][j]*fator_compressao;
    }
}


/* ----- Funções principais ----- */


//Função que realiza o processo de quantização nos blocos 8x8 de uma imagem.
//Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
//da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
//indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos 8x8 nos canais de luminância
//e crominância.
//Saída: nenhuma.
void quantizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    //Quantização do canal Y.
    for(int i = 0; i < qtd_blocos_y; i++)
        divide_bloco(blocos8x8[0][i], quantizacao_luminancia);

    //Quantização dos canais Cb e Cr.
    for(int i = 0; i < qtd_blocos_c; i++) {
        divide_bloco(blocos8x8[1][i], quantizacao_crominancia);
        divide_bloco(blocos8x8[2][i], quantizacao_crominancia);
    }
}

//Função que desfaz (com perdas de informação) o processo de quantização dos blocos 8x8 de uma imagem.
//Entrada: ponteiro quádruplo para double segundo a entrada da função de quantização; quantidade de blocos nos
//canais de luminância e crominância.
//Saída: nenhuma
void quantizacao_inversa(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    //Quantização inversa do canal Y.
    for(int i = 0; i < qtd_blocos_y; i++)
        multiplica_bloco(blocos8x8[0][i], quantizacao_luminancia);

    //Quantização inversa dos canais Cb e Cr.
    for(int i = 0; i < qtd_blocos_c; i++) {
        multiplica_bloco(blocos8x8[1][i], quantizacao_crominancia);
        multiplica_bloco(blocos8x8[2][i], quantizacao_crominancia);
    }
}