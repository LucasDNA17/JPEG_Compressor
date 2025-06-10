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

#pragma pack(push, 1) // Ajusta o alinhamento para 1 byte
// Estrutura para cabeçalho do arquivo comprimido
typedef struct {
    char assinatura[4];      // "BINC" - assinatura do arquivo binario comprimido
    int largura;             // largura da imagem original
    int altura;              // altura da imagem original
    int largura_c;           // largura das crominâncias (com padding)
    int altura_c;            // altura das crominâncias (com padding)
    int qtd_blocos_y;        // quantidade de blocos Y
    int qtd_blocos_c;        // quantidade de blocos C
    int tamanho_dados;       // tamanho dos dados comprimidos
} Cabecalho_Comprimido;
#pragma pack(pop)

// Estrutura para dados de um bloco comprimido
typedef struct {
    int dc_diferenca;        // diferença DC
    int num_simbolos_ac;     // número de símbolos AC
    unsigned char *dados_bits; // dados binários dos códigos
    int tamanho_bits;        // tamanho em bits dos dados
} Bloco_Comprimido;

// Funções para manipulação de buffer de bits (apenas as públicas)
Buffer_Bits *criar_buffer_bits(int capacidade_inicial);
void liberar_buffer_bits(Buffer_Bits *buffer);

// Funções públicas principais para salvar/carregar arquivo comprimido
int salvar_imagem_comprimida(const char *nome_arquivo, Bloco_Huffman ***blocos_huffman, int largura, int altura, int largura_c, int altura_c, int qtd_blocos_y, int qtd_blocos_c);

Bloco_Huffman ***carregar_imagem_comprimida(const char *nome_arquivo, int *largura, int *altura, int *largura_c, int *altura_c, int *qtd_blocos_y, int *qtd_blocos_c);

// Função para calcular taxa de compressão
void calcular_estatisticas_compressao(const char *nome_original, const char *nome_comprimido);

// Função para descompressão completa - reverte todo o pipeline
int descompressao_completa_bin_para_bmp(const char *arquivo_bin, const char *arquivo_bmp_saida);

#endif 