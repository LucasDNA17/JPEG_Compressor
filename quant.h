#ifndef QUANT_H
    #define QUANT_H

    #include "matrix.h"

    //Fator que determina a qualidade (e compressão) da imagem resultante.
    //Derivada por meio de testes empíricos: valores maiores ou iguais a este
    //causavam perdas bastante visíveis de qualidade.
    #define fator_compressao 1.5

    //Função que realiza o processo de quantização nos blocos 8x8 de uma imagem.
    //Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
    //da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
    //indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos 8x8 nos canais de luminância
    //e crominância.
    //Saída: nenhuma.
    void quantizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);

    //Função que desfaz (com perdas de informação) o processo de quantização dos blocos 8x8 de uma imagem.
    //Entrada: ponteiro quádruplo para double segundo a entrada da função de quantização; quantidade de blocos nos
    //canais de luminância e crominância.
    //Saída: nenhuma
    void quantizacao_inversa(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);

#endif