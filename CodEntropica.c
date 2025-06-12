#include "CodEntropica.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//Matriz de índices pré-calculada em que o elemento a_ij representa
//a posição do coeficiente c_ij do bloco no vetor final, seguindo o padrão zig-zag.
int matriz_indices_vetorizacao[8][8] = {
    {0, 1, 5 ,6, 14, 15, 27, 28},
    {2, 4, 7, 13, 16, 26, 29, 42},
    {3, 8, 12, 17, 25, 30, 41, 43},
    {9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
};


//Tabelas DC
const uint8_t huffman_dc_len[12] = { 2,3,3,3,3,3,4,5,6,7,8,9 };
const uint16_t huffman_dc_code[12] = { 
    0x000,0x002,0x003,0x004,0x005,0x006,
    0x00e,0x01e,0x03e,0x07e,0x0fe,0x1fe 
};

//Tabelas AC
const uint8_t huffman_ac_len[256] = {
     4, 2, 2, 3, 4, 5, 7, 8,
    10,16,16, 0, 0, 0, 0, 0,
     0, 4, 5, 7, 9,11,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 5, 8,10,12,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 6, 9,12,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 6,10,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 7,11,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 7,12,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 8,12,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 9,15,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 9,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0, 9,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0,10,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0,10,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0,11,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
     0,16,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0,
    11,16,16,16,16,16,16,16,
    16,16,16, 0, 0, 0, 0, 0
};

const uint16_t huffman_ac_code[256] = {
    0x000a,0x0000,0x0001,0x0004,0x000b,0x001a,0x0078,0x00f8,
    0x03f6,0xff82,0xff83,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x000c,0x001b,0x0079,0x01f6,0x07f6,0xff84,0xff85,
    0xff86,0xff87,0xff88,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x001c,0x00f9,0x03f7,0x0ff4,0xff89,0xff8a,0xff8b,
    0xff8c,0xff8d,0xff8e,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x003a,0x01f7,0x0ff5,0xff8f,0xff90,0xff91,0xff92,
    0xff93,0xff94,0xff95,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x003b,0x03f8,0xff96,0xff97,0xff98,0xff99,0xff9a,
    0xff9b,0xff9c,0xff9d,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x007a,0x07f7,0xff9e,0xff9f,0xffa0,0xffa1,0xffa2,
    0xffa3,0xffa4,0xffa5,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x007b,0x0ff6,0xffa6,0xffa7,0xffa8,0xffa9,0xffaa,
    0xffab,0xffac,0xffad,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x00fa,0x0ff7,0xffae,0xffaf,0xffb0,0xffb1,0xffb2,
    0xffb3,0xffb4,0xffb5,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x01f8,0x7fc0,0xffb6,0xffb7,0xffb8,0xffb9,0xffba,
    0xffbb,0xffbc,0xffbd,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x01f9,0xffbe,0xffbf,0xffc0,0xffc1,0xffc2,0xffc3,
    0xffc4,0xffc5,0xffc6,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x01fa,0xffc7,0xffc8,0xffc9,0xffca,0xffcb,0xffcc,
    0xffcd,0xffce,0xffcf,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x03f9,0xffd0,0xffd1,0xffd2,0xffd3,0xffd4,0xffd5,
    0xffd6,0xffd7,0xffd8,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x03fa,0xffd9,0xffda,0xffdb,0xffdc,0xffdd,0xffde,
    0xffdf,0xffe0,0xffe1,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x07f8,0xffe2,0xffe3,0xffe4,0xffe5,0xffe6,0xffe7,
    0xffe8,0xffe9,0xffea,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0xffeb,0xffec,0xffed,0xffee,0xffef,0xfff0,0xfff1,
    0xfff2,0xfff3,0xfff4,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x07f9,0xfff5,0xfff6,0xfff7,0xfff8,0xfff9,0xfffa,0xfffb,
    0xfffc,0xfffd,0xfffe,0x0000,0x0000,0x0000,0x0000,0x0000
};



//Função que transforma um bloco 8x8 em vetor seguindo padrao zigue-zague.
//Entrada: ponteiro duplo para double que representa o bloco 8x8 a ser vetorizado.
//Saída: ponteiro para vetor de 64 elementos ordenados.
int *vetorizacao_bloco8x8(double **bloco8x8) {
    if(bloco8x8 == NULL) return NULL;

    //Cópia dos elementos do bloco 8x8 para o vetor, sendo a posição
    //dos coeficientes determinada pela matriz de índices.
    int *vetor = (int *) malloc(64*sizeof(int));
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            vetor[matriz_indices_vetorizacao[i][j]] = (int) bloco8x8[i][j];
    
    //Desaloca o bloco 8x8 e retorna o vetor criado.
    desaloca_matrix((void **) bloco8x8, 8, 8);
    return vetor;    
}

//Função que reconstrói bloco 8x8 a partir de vetor ordenado zigue-zague.
//Entrada: vetor de 64 coeficientes ordenados.
//Saída: ponteiro para matriz 8x8 reconstruida.
double **matricizacao_bloco8x8(int *vetor) {
    if(vetor == NULL) return NULL;

    //Cópia dos elementos do vetor para o bloco 8x8, sendo a posição
    //dos coeficientes determinada pela matriz de índices.
    double **bloco8x8 = (double **) aloca_matrix(1, 8, 8);
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            bloco8x8[i][j] = (double) vetor[matriz_indices_vetorizacao[i][j]];

    //Desaloca o vetor de coeficientes e retorna o bloco 8x8 criado.
    free(vetor);
    return bloco8x8;    
}

//Função que vetoriza todos os blocos 8x8 de uma imagem.
//Entrada: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
//da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
//indexações representam o bloco 8x8 em si, no formato de matriz; quantidade de blocos nos canais de crominância
//e luminância; quantidade de blocos dos canais de luminância e crominância.
//Saída: ponteiro para estrutura de vetores por canal de informação.
int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos8x8 == NULL) return NULL;

    //Lista que armazena as informações dos três canais de informação da imagem.
    int ***vetores = (int ***) malloc(3*sizeof(int **));
    //Lista de vetores do canal Y.
    vetores[0] = (int **) malloc(qtd_blocos_y*sizeof(int *));
    //Lista de vetores do canal Cb.
    vetores[1] = (int **) malloc(qtd_blocos_c*sizeof(int *));
    //Lista de vetores do canal Cr.
    vetores[2] = (int **) malloc(qtd_blocos_c*sizeof(int *));

    //Vetorização bloco a bloco do canal Y.
    for(int i = 0; i < qtd_blocos_y; i++)
        vetores[0][i] = vetorizacao_bloco8x8(blocos8x8[0][i]);

    //Vetorização bloco a bloco dos canais Cb e Cr.
    for(int i = 0; i < qtd_blocos_c; i++) {
        vetores[1][i] = vetorizacao_bloco8x8(blocos8x8[1][i]); //Canal Cb.
        vetores[2][i] = vetorizacao_bloco8x8(blocos8x8[2][i]); //Canal Cr.
    }

    //Desaloca as listas de blocos de todos os canais de informação.
    free(blocos8x8[0]); free(blocos8x8[1]); free(blocos8x8[2]);
    free(blocos8x8);

    return vetores;
}

//Função que reconstrói todos os blocos 8x8 a partir dos vetores correspondentes.
//Entrada: conjunto de vetores por canal de inforamção; quantidade de blocos dos canais
//de luminância e crominância.
//Saída: ponteiro para estrutura de blocos 8x8 por canal (igual à entrada da função de vetorização).
double ****matricizacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c) {
    if(vetor == NULL) return NULL;

    //Lista que armazena os três canais de informação da imagem.
    double ****blocos8x8 = (double ****) malloc(3*sizeof(double ***));

    //Lista de blocos 8x8 do canal Y.
    blocos8x8[0] = (double ***) malloc(qtd_blocos_y*sizeof(double **));
    //Lista de blocos 8x8 do canal Cb.
    blocos8x8[1] = (double ***) malloc(qtd_blocos_c*sizeof(double **));
    //Lista de blocos 8x8 do canal Cr.
    blocos8x8[2] = (double ***) malloc(qtd_blocos_c*sizeof(double **));

    //Matricização bloco a bloco do canal Y.
    for(int i = 0; i < qtd_blocos_y; i++)
        blocos8x8[0][i] = matricizacao_bloco8x8(vetor[0][i]);

    //Matricização bloco a bloco dos canais Cb e Cr.
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos8x8[1][i] = matricizacao_bloco8x8(vetor[1][i]);
        blocos8x8[2][i] = matricizacao_bloco8x8(vetor[2][i]);
    }

    //Desaloca as listas de vetores dos trẽs canais de informação.
    free(vetor[0]); free(vetor[1]); free(vetor[2]);
    free(vetor);

    return blocos8x8;
}


//Codifica um vetor de coeficientes usando Run-Length Encoding
//Entrada: vetor - vetor de 64 coeficientes DCT
//Saída: ponteiro para bloco RLE codificado
Bloco_RLE *codificar_bloco_rle(int *vetor) {
    if(vetor == NULL) return NULL;
    
    Bloco_RLE *bloco_rle = (Bloco_RLE *) malloc(sizeof(Bloco_RLE));
    
    // O primeiro elemento eh sempre o coeficiente DC
    bloco_rle->dc = vetor[0];
    
    // Aloca espaco para no max 63 pares AC
    int max_pares = 63;
    bloco_rle->pares_ac = (Par_RLE *) malloc(max_pares * sizeof(Par_RLE));
    bloco_rle->num_pares = 0;
    
    int zeros = 0;  // Conta os zeros consecutivos
    
    // Processa coeficientes AC (indices 1-63)
    for(int i = 1; i < 64; i++) {
        if(vetor[i] == 0) {
            zeros++;
        } else {
            // Encontrou um valor nao zero
            bloco_rle->pares_ac[bloco_rle->num_pares].zeros_consecutivos = zeros;
            bloco_rle->pares_ac[bloco_rle->num_pares].amplitude = vetor[i];
            bloco_rle->num_pares++;
            zeros = 0;
        }
    }
    
    // Se ha zeros no final, adiciona marcador de fim de bloco (0,0)
    if(zeros > 0 || bloco_rle->num_pares == 0) {
        bloco_rle->pares_ac[bloco_rle->num_pares].zeros_consecutivos = 0;
        bloco_rle->pares_ac[bloco_rle->num_pares].amplitude = 0;
        bloco_rle->num_pares++;
    }
    
    // Redimensiona para o tamanho exato
    bloco_rle->pares_ac = (Par_RLE *) realloc(bloco_rle->pares_ac, bloco_rle->num_pares * sizeof(Par_RLE));
    
    return bloco_rle;
}

//Decodifica um bloco RLE de volta para vetor de coeficientes
//Entrada: bloco_rle - bloco codificado em RLE
//Saída: ponteiro para vetor de 64 coeficientes decodificados
int *decodificar_bloco_rle(Bloco_RLE *bloco_rle) {
    if(bloco_rle == NULL) return NULL;
    
    int *vetor = (int *) calloc(64, sizeof(int));
    
    // Primeiro elemento eh o DC
    vetor[0] = bloco_rle->dc;
    
    int pos = 1;  // posicao atual no vetor (comecando nos AC)
    
    // Processa pares RLE
    for(int i = 0; i < bloco_rle->num_pares; i++) {
        int zeros = bloco_rle->pares_ac[i].zeros_consecutivos;
        int amplitude = bloco_rle->pares_ac[i].amplitude;
        
        // Se encontrou marcador de fim de bloco (0,0), para o processamento
        if(zeros == 0 && amplitude == 0) {
            break;
        }
        
        // Pula 'zeros' zeros (ja inicializados como zero)
        pos += zeros;
        
        // Coloca a amplitude na posicao atual
        if(pos < 64) {
            vetor[pos] = amplitude;
            pos++;
        }
    }
    
    liberar_bloco_rle(bloco_rle);
    return vetor;
}

//Aplica codificacao RLE a todos os vetores da imagem
//Entrada: vetores - estrutura de vetores por canal, qtd_blocos_y - quantidade de blocos Y, qtd_blocos_c - quantidade de blocos Cb/Cr
//Saída: ponteiro para estrutura de blocos RLE por canal
Bloco_RLE ***codificar_rle(int ***vetores, int qtd_blocos_y, int qtd_blocos_c) {
    if(vetores == NULL) return NULL;
    
    Bloco_RLE ***blocos_rle = (Bloco_RLE ***) malloc(3*sizeof(Bloco_RLE **));
    
    // Canal Y
    blocos_rle[0] = (Bloco_RLE **) malloc(qtd_blocos_y*sizeof(Bloco_RLE *));
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos_rle[0][i] = codificar_bloco_rle(vetores[0][i]);
        free(vetores[0][i]);
    }
    
    // Canais Cb e Cr
    blocos_rle[1] = (Bloco_RLE **) malloc(qtd_blocos_c*sizeof(Bloco_RLE *));
    blocos_rle[2] = (Bloco_RLE **) malloc(qtd_blocos_c*sizeof(Bloco_RLE *));
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_rle[1][i] = codificar_bloco_rle(vetores[1][i]);
        blocos_rle[2][i] = codificar_bloco_rle(vetores[2][i]);
        free(vetores[1][i]);
        free(vetores[2][i]);
    }
    
    free(vetores[0]); free(vetores[1]); free(vetores[2]);
    free(vetores);
    return blocos_rle;
}

//Decodifica todos os blocos RLE de volta para vetores
//Entrada: blocos_rle - estrutura de blocos RLE por canal, qtd_blocos_y - quantidade de blocos Y, qtd_blocos_c - quantidade de blocos Cb/Cr
//Saída: ponteiro para estrutura de vetores decodificados por canal
int ***decodificar_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_rle == NULL) return NULL;
    
    int ***vetores = (int ***) malloc(3*sizeof(int **));
    
    // Canal Y
    vetores[0] = (int **) malloc(qtd_blocos_y*sizeof(int *));
    for(int i = 0; i < qtd_blocos_y; i++) {
        vetores[0][i] = decodificar_bloco_rle(blocos_rle[0][i]);
    }
    
    // Canais Cb e Cr
    vetores[1] = (int **) malloc(qtd_blocos_c*sizeof(int *));
    vetores[2] = (int **) malloc(qtd_blocos_c*sizeof(int *));
    for(int i = 0; i < qtd_blocos_c; i++) {
        vetores[1][i] = decodificar_bloco_rle(blocos_rle[1][i]);
        vetores[2][i] = decodificar_bloco_rle(blocos_rle[2][i]);
    }
    
    free(blocos_rle[0]); free(blocos_rle[1]); free(blocos_rle[2]);
    free(blocos_rle);
    return vetores;
}

//Libera memoria alocada para um bloco RLE
//Entrada: bloco_rle - ponteiro para bloco RLE
//Saída: nenhuma
void liberar_bloco_rle(Bloco_RLE *bloco_rle) {
    if(bloco_rle != NULL) {
        if(bloco_rle->pares_ac != NULL) {
            free(bloco_rle->pares_ac);
        }
        free(bloco_rle);
    }
}

//Libera memoria de todos os blocos RLE da imagem
//Entrada: blocos_rle - estrutura de blocos RLE, qtd_blocos_y - quantidade de blocos Y, qtd_blocos_c - quantidade de blocos Cb/Cr
//Saída: nenhuma
void liberar_blocos_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_rle == NULL) return;
    
    // Libera canal Y
    for(int i = 0; i < qtd_blocos_y; i++) {
        liberar_bloco_rle(blocos_rle[0][i]);
    }
    free(blocos_rle[0]);
    
    // Libera canais Cb e Cr
    for(int i = 0; i < qtd_blocos_c; i++) {
        liberar_bloco_rle(blocos_rle[1][i]);
        liberar_bloco_rle(blocos_rle[2][i]);
    }
    free(blocos_rle[1]);
    free(blocos_rle[2]);
    free(blocos_rle);
}



//Obtem codigo de amplitude JPEG para um valor e categoria
//Entrada: valor - valor a ser codificado, categoria - categoria do valor
//Saída: codigo de amplitude correspondente
int obter_codigo_amplitude(int valor, int categoria) {
    if(categoria == 0) return 0;
    
    if(valor > 0) {
        return valor;
    } else {
        // Para valores negativos, usa complemento
        return valor + (1 << categoria) - 1;
    }
}

//Codifica um bloco RLE usando representacao Huffman
//Entrada: bloco_rle - bloco RLE a ser codificado, dc_anterior - valor DC do bloco anterior
//Saída: ponteiro para bloco Huffman codificado
Bloco_Huffman *codificar_bloco_huffman(Bloco_RLE *bloco_rle, int dc_anterior) {
    if(bloco_rle == NULL) return NULL;
    
    Bloco_Huffman *bloco_huff = (Bloco_Huffman *) malloc(sizeof(Bloco_Huffman));
    
    // Calcula diferença DC usando DPCM
    bloco_huff->diferenca_dc = bloco_rle->dc - dc_anterior;
    bloco_huff->tamanho_original = bloco_rle->num_pares;
    
    // Cria cópia dos dados originais para compatibilidade com Compressao.c
    bloco_huff->dados_originais = (Bloco_RLE *) malloc(sizeof(Bloco_RLE));
    bloco_huff->dados_originais->dc = bloco_rle->dc;
    bloco_huff->dados_originais->num_pares = bloco_rle->num_pares;
    
    if(bloco_rle->num_pares > 0 && bloco_rle->pares_ac != NULL) {
        bloco_huff->dados_originais->pares_ac = (Par_RLE *) malloc(bloco_rle->num_pares * sizeof(Par_RLE));
        for(int i = 0; i < bloco_rle->num_pares; i++) {
            bloco_huff->dados_originais->pares_ac[i] = bloco_rle->pares_ac[i];
        }
    } else {
        bloco_huff->dados_originais->pares_ac = NULL;
    }
    
    return bloco_huff;
}

Bloco_RLE *decodificar_bloco_huffman(Bloco_Huffman *bloco_huff, int *dc_anterior) {
    if(bloco_huff == NULL || dc_anterior == NULL) return NULL;
    
    // Reconstrói bloco RLE a partir dos dados originais
    Bloco_RLE *bloco_rle = (Bloco_RLE *) malloc(sizeof(Bloco_RLE));
    
    // Reconstrói DC usando DPCM
    bloco_rle->dc = *dc_anterior + bloco_huff->diferenca_dc;
    *dc_anterior = bloco_rle->dc;
    
    // Copia dados AC dos dados originais
    bloco_rle->num_pares = bloco_huff->dados_originais->num_pares;
    
    if(bloco_rle->num_pares > 0 && bloco_huff->dados_originais->pares_ac != NULL) {
        bloco_rle->pares_ac = (Par_RLE *) malloc(bloco_rle->num_pares * sizeof(Par_RLE));
        for(int i = 0; i < bloco_rle->num_pares; i++) {
            bloco_rle->pares_ac[i] = bloco_huff->dados_originais->pares_ac[i];
        }
    } else {
        bloco_rle->pares_ac = NULL;
        bloco_rle->num_pares = 0;
    }
    
    liberar_bloco_huffman(bloco_huff);
    return bloco_rle;
}

//Aplica codificacao Huffman a todos os blocos RLE da imagem
//Entrada: blocos_rle - estrutura de blocos RLE por canal, qtd_blocos_y - quantidade de blocos Y, qtd_blocos_c - quantidade de blocos Cb/Cr
//Saída: ponteiro para estrutura de blocos Huffman por canal
Bloco_Huffman ***codificar_huffman(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_rle == NULL) return NULL;
    
    Bloco_Huffman ***blocos_huff = (Bloco_Huffman ***) malloc(3*sizeof(Bloco_Huffman **));
    
    // Canal Y
    blocos_huff[0] = (Bloco_Huffman **) malloc(qtd_blocos_y*sizeof(Bloco_Huffman *));
    int dc_anterior_y = 0;
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos_huff[0][i] = codificar_bloco_huffman(blocos_rle[0][i], dc_anterior_y);
        dc_anterior_y = blocos_rle[0][i]->dc;
        liberar_bloco_rle(blocos_rle[0][i]);
    }
    
    // Canais Cb e Cr
    blocos_huff[1] = (Bloco_Huffman **) malloc(qtd_blocos_c*sizeof(Bloco_Huffman *));
    blocos_huff[2] = (Bloco_Huffman **) malloc(qtd_blocos_c*sizeof(Bloco_Huffman *));
    
    int dc_anterior_cb = 0, dc_anterior_cr = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_huff[1][i] = codificar_bloco_huffman(blocos_rle[1][i], dc_anterior_cb);
        blocos_huff[2][i] = codificar_bloco_huffman(blocos_rle[2][i], dc_anterior_cr);
        dc_anterior_cb = blocos_rle[1][i]->dc;
        dc_anterior_cr = blocos_rle[2][i]->dc;
        liberar_bloco_rle(blocos_rle[1][i]); 
        liberar_bloco_rle(blocos_rle[2][i]); 
    }
    
    free(blocos_rle[0]); free(blocos_rle[1]); free(blocos_rle[2]);
    free(blocos_rle); 
    return blocos_huff;
}

//Decodifica todos os blocos Huffman de volta para blocos RLE
//Entrada: blocos_huff - estrutura de blocos Huffman por canal, qtd_blocos_y - quantidade de blocos Y, qtd_blocos_c - quantidade de blocos Cb/Cr
//Saída: ponteiro para estrutura de blocos RLE decodificados por canal
Bloco_RLE ***decodificar_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_huff == NULL) return NULL;
    
    Bloco_RLE ***blocos_rle = (Bloco_RLE ***) malloc(3*sizeof(Bloco_RLE **));
    
    // Canal Y
    blocos_rle[0] = (Bloco_RLE **) malloc(qtd_blocos_y*sizeof(Bloco_RLE *));
    int dc_anterior_y = 0;
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos_rle[0][i] = decodificar_bloco_huffman(blocos_huff[0][i], &dc_anterior_y);
    }
    
    // Canais Cb e Cr
    blocos_rle[1] = (Bloco_RLE **) malloc(qtd_blocos_c*sizeof(Bloco_RLE *));
    blocos_rle[2] = (Bloco_RLE **) malloc(qtd_blocos_c*sizeof(Bloco_RLE *));
    
    int dc_anterior_cb = 0, dc_anterior_cr = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_rle[1][i] = decodificar_bloco_huffman(blocos_huff[1][i], &dc_anterior_cb);
        blocos_rle[2][i] = decodificar_bloco_huffman(blocos_huff[2][i], &dc_anterior_cr);
    }
    
    free(blocos_huff[0]); free(blocos_huff[1]); free(blocos_huff[2]);
    free(blocos_huff);
    return blocos_rle;
}

//Libera memoria alocada para um bloco Huffman
//Entrada: bloco_huff - ponteiro para bloco Huffman
//Saída: Saída
void liberar_bloco_huffman(Bloco_Huffman *bloco_huff) {
    if(bloco_huff != NULL) {
        if(bloco_huff->dados_originais != NULL) {
            liberar_bloco_rle(bloco_huff->dados_originais);
        }
        free(bloco_huff);
    }
}

//Libera memoria de todos os blocos Huffman da imagem
//Entrada: blocos_huff - estrutura de blocos Huffman, qtd_blocos_y - quantidade de blocos Y, qtd_blocos_c - quantidade de blocos Cb/Cr
//Saída: nenhuma
void liberar_blocos_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_huff == NULL) return;
    
    // Libera canal Y
    for(int i = 0; i < qtd_blocos_y; i++) {
        liberar_bloco_huffman(blocos_huff[0][i]);
    }
    free(blocos_huff[0]);
    
    // Libera canais Cb e Cr
    for(int i = 0; i < qtd_blocos_c; i++) {
        liberar_bloco_huffman(blocos_huff[1][i]);
        liberar_bloco_huffman(blocos_huff[2][i]);
    }
    free(blocos_huff[1]);
    free(blocos_huff[2]);
    free(blocos_huff);
}


