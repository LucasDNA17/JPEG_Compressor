#ifndef COD_ENTROPICA_H
    #define COD_ENTROPICA_H

    #include "matrix.h"

    // Estrutura para pares RLE
    typedef struct {
        int zeros_consecutivos;  // quantos zeros consecutivos
        int amplitude;           // valor do coeficiente nao zero
    } Par_RLE;

    // Estrutura para um bloco RLE
    typedef struct {
        int dc;                  // coeficiente DC
        Par_RLE *pares_ac;       // array com pares AC
        int num_pares;           // numero de pares AC
    } Bloco_RLE;

    // Estrutura para Huffman
    typedef struct {
        int diferenca_dc;         // diferenca DC (DPCM)
        //int total_bits;           // total de bits apos compressao
        Bloco_RLE *dados_originais; // dados originais para decodificacao
    } Bloco_Huffman;

    // Funcoes para vetorizacao
    int *vetorizacao_bloco8x8(double **bloco8x8);
    double **matrizicacao_bloco8x8(int *vetor);
    int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);
    double ****matrizicacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c);
    
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