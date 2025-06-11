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


    /* ----- Funções de vetorização/matricização de blocos 8x8 ----- */


    //Função que vetoriza todos os blocos 8x8 de uma imagem.
    //Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
    //da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
    //indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos nos canais de crominância
    //e luminância; quantidade de blocos dos canais de luminância e crominância.
    //Saída: ponteiro para estrutura de vetores por canal de informação.
    int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c);


    //Função que reconstrói todos os blocos 8x8 a partir dos vetores correspondentes.
    //Entrada: conjunto de vetores por canal de inforamção; quantidade de blocos dos canais
    //de luminância e crominância.
    //Saída: ponteiro para estrutura de blocos 8x8 por canal (igual à entrada da função de vetorização).
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