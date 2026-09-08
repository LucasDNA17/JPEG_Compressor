#ifndef DCT_H
    #define DCT_H

    #include "../matrix/matrix.h"
    #include "../BMP_ColorElements/BMP_ColorElements.h"

    //Função que realiza a DCT nos blocos 8x8 de uma imagem.
    //Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
    //da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
    //indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos nos canais de crominância
    //e luminância.
    //Saída: nenhuma.
    void DCT (double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);

    //Função que realiza a DCT inversa nos blocos 8x8 de uma imagem.
    //Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
    //da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
    //indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos nos canais de crominância
    //e luminância.
    //Saída: nenhuma.
    void DCT_inversa(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);

#endif