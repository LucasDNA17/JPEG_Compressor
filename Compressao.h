#ifndef COMPRESSAO_H
#define COMPRESSAO_H

#include "CodEntropica.h"
#include <stdio.h>

// Estrutura para buffer de bits
typedef struct {
    unsigned char *dados;    // buffer para dados binários
    int tamanho_buffer;      // tamanho atual do buffer
    int capacidade;          // capacidade máxima do buffer
    int bit_atual;           // posição do bit atual (0-7)
    int byte_leitura;        // posição do byte para leitura
    int bit_leitura;         // posição do bit para leitura
} Buffer_Bits;

// Estrutura para cabeçalho do arquivo comprimido
typedef struct {
    char assinatura[4];      // "BINC" - assinatura do arquivo binario comprimido
    int largura;             // largura da imagem
    int altura;              // altura da imagem
    int qtd_blocos_y;        // quantidade de blocos Y
    int qtd_blocos_c;        // quantidade de blocos C
    int tamanho_dados;       // tamanho dos dados comprimidos
} Cabecalho_Comprimido;

// Estrutura para dados de um bloco comprimido
typedef struct {
    int dc_diferenca;        // diferença DC (DPCM)
    int num_simbolos_ac;     // número de símbolos AC
    unsigned char *dados_bits; // dados binários dos códigos
    int tamanho_bits;        // tamanho em bits dos dados
} Bloco_Comprimido;

// Funções para manipulação de buffer de bits
Buffer_Bits *criar_buffer_bits(int capacidade_inicial);
void liberar_buffer_bits(Buffer_Bits *buffer);
void escrever_bit(Buffer_Bits *buffer, int bit);
void escrever_bits(Buffer_Bits *buffer, int valor, int num_bits);
int ler_bit(Buffer_Bits *buffer);
int ler_bits(Buffer_Bits *buffer, int num_bits);
void resetar_posicao_leitura(Buffer_Bits *buffer);

// Funções para compressão de blocos
Bloco_Comprimido *comprimir_bloco_huffman(Bloco_Huffman *bloco_huff);
Bloco_Huffman *descomprimir_bloco_huffman(Bloco_Comprimido *bloco_comp, int dc_anterior);
void liberar_bloco_comprimido(Bloco_Comprimido *bloco_comp);

// Funções para salvar/carregar arquivo comprimido
int salvar_imagem_comprimida(const char *nome_arquivo, Bloco_Huffman ***blocos_huffman, int largura, int altura, int qtd_blocos_y, int qtd_blocos_c);

Bloco_Huffman ***carregar_imagem_comprimida(const char *nome_arquivo, int *largura, int *altura, int *qtd_blocos_y, int *qtd_blocos_c);

// Funções auxiliares para codificação Huffman 
void codificar_categoria_amplitude(Buffer_Bits *buffer, int valor);
int decodificar_categoria_amplitude(Buffer_Bits *buffer);
void codificar_simbolo_ac(Buffer_Bits *buffer, int zeros, int amplitude);
void decodificar_simbolo_ac(Buffer_Bits *buffer, int *zeros, int *amplitude);

// Função para calcular taxa de compressão
void calcular_estatisticas_compressao(const char *nome_original, const char *nome_comprimido);

// Função para descompressão completa - reverte todo o pipeline
int descompressao_completa_bin_para_bmp(const char *arquivo_bin, const char *arquivo_bmp_saida);

#endif 