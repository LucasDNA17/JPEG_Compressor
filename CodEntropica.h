#ifndef COD_ENTROPICA_H
    #define COD_ENTROPICA_H

    #include "matrix.h"

    // Estrutura para pares RLE
    typedef struct {
        int zeros_consecutivos;  // quantidade de zeros consecutivos
        int amplitude;           // valor do coeficiente não nulo
    } Par_RLE;

    // Estrutura para um bloco RLE
    typedef struct {
        int dc;                  // coeficiente DC
        Par_RLE *pares_ac;       // array com pares AC
        int num_pares;           // número de pares AC
    } Bloco_RLE;

    // Estrutura para Huffman
    typedef struct {
        int diferenca_dc;         // diferença DC (DPCM)
        //int total_bits;           // total de bits após compressão
        Bloco_RLE *dados_originais; // dados originais para decodificação
    } Bloco_Huffman;


    /* ----- Funções de vetorização/matricização de imagens ----- */

    //Função que transforma os blocos 8x8 de uma imagem em vetores. A vetorização segue
    //o padrão "zigue-zague" necessário à compressão JPEG.
    //Entrada: ponteiro quádruplo (para double) em que a primeira indexação representa o canal de informação
    //da imagem (Y - 0; Cb - 1; Cr - 2), a segunda representa o bloco 8x8 do canal (1º, 2º, etc.) e as duas
    //últimas representam a posição do pixel no bloco 8x8, entendido como uma matriz; quantidade de blocos no
    //canal Y; quantidade de blocos nos canais de crominância Cb e Cr.
    //Saída: Ponteiro triplo para inteiro em que a primeira indexação representa o canal de informação da imagem
    //(Y - 0; 1 - Cb; 2 - Cr), a segunda qual é o bloco 8x8 vetorizado (1º, 2º, etc.), e a terceira as posições dos
    //pixels no vetor. 
    int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);

    //Função que desfaz o processo de vetorização, gerando novamente os blocos 8x8 em formato de matriz.
    //Entrada: ponteiro triplo para inteiro, em que a indexação é idêntica à saída da função "vetorizacao".
    //Saída: ponteiro quádruplo para double em que a indexação é idêntica à entrada da função "vetorizacao".
    double ****matricizacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c);
    
    // Funcoes RLE
    Bloco_RLE *codificar_bloco_rle(int *vetor);
    int *decodificar_bloco_rle(Bloco_RLE *bloco_rle);
    Bloco_RLE ***codificar_rle(int ***vetores, int qtd_blocos_y, int qtd_blocos_c);
    int ***decodificar_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c);
    void liberar_bloco_rle(Bloco_RLE *bloco_rle);
    void liberar_blocos_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c);
    void imprimir_debug_rle(Bloco_RLE *bloco_rle);

    // Funcoes Huffman simplificadas
    Bloco_Huffman *codificar_bloco_huffman(Bloco_RLE *bloco_rle, int dc_anterior);
    Bloco_RLE *decodificar_bloco_huffman(Bloco_Huffman *bloco_huff, int *dc_anterior);
    Bloco_Huffman ***codificar_huffman(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c);
    Bloco_RLE ***decodificar_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c);
    void liberar_bloco_huffman(Bloco_Huffman *bloco_huff);
    void liberar_blocos_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c);

#endif