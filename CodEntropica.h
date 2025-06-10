#ifndef COD_ENTROPICA_H
    #define COD_ENTROPICA_H

    #include "matrix.h"
    #include <stdint.h>

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

    // Buffer_Bits definido em Compressao.h

    // Estrutura para Huffman (compatível com Compressao.c)
    typedef struct {
        int diferenca_dc;                // diferença DC (DPCM) 
        Bloco_RLE *dados_originais;      // dados originais para decodificação
        int tamanho_original;            // tamanho original para verificação
    } Bloco_Huffman;

    // Tabelas Huffman padrão
    extern const uint8_t huffman_dc_len[12];
    extern const uint16_t huffman_dc_code[12];
    extern const uint8_t huffman_ac_len[256];
    extern const uint16_t huffman_ac_code[256];


    int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);
    double ****matricizacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c);
    Bloco_RLE *codificar_bloco_rle(int *vetor);
    int *decodificar_bloco_rle(Bloco_RLE *bloco_rle);
    Bloco_RLE ***codificar_rle(int ***vetores, int qtd_blocos_y, int qtd_blocos_c);
    int ***decodificar_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c);
    void liberar_bloco_rle(Bloco_RLE *bloco_rle);
    void liberar_blocos_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c);
    void imprimir_debug_rle(Bloco_RLE *bloco_rle);
    Bloco_Huffman *codificar_bloco_huffman(Bloco_RLE *bloco_rle, int dc_anterior);
    Bloco_RLE *decodificar_bloco_huffman(Bloco_Huffman *bloco_huff, int *dc_anterior);
    Bloco_Huffman ***codificar_huffman(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c);
    Bloco_RLE ***decodificar_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c);
    void liberar_bloco_huffman(Bloco_Huffman *bloco_huff);
    void liberar_blocos_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c);
    int obter_codigo_amplitude(int valor, int categoria);

#endif