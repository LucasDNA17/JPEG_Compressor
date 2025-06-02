#include "CodEntropica.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


/* ----- Funções de vetorização/matricização de imagens ----- */

//Função que transforma um bloco 8x8 de uma imagem em vetor. A vetorização segue
//o padrão "zigue-zague" necessário à compressão JPEG. A função desaloca o espaço
//de memória do bloco 8x8.
 int *vetorizacao_bloco8x8(double **bloco8x8) {
    if(bloco8x8 == NULL) return NULL;

    //Matriz de índices pré-calculada em que o elemento a_ij representa
    //a posição do coeficiente c_ij do bloco no vetor final.
    int index_matrix[8][8] = {
    {0, 1, 5 ,6, 14, 15, 27, 28},
    {2, 4, 7, 13, 16, 26, 29, 42},
    {3, 8, 12, 17, 25, 30, 41, 43},
    {9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
    };

    //Cópia dos elementos do bloco 8x8 para o vetor, sendo a posição
    //dos coeficientes determinada pela matriz de índices.
    int *vetor = (int *) malloc(64*sizeof(int));
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            vetor[index_matrix[i][j]] = (int) bloco8x8[i][j];
    
    //Desaloca o bloco 8x8 e retorna o vetor criado.
    desaloca_matrix((void **) bloco8x8, 8, 8);
    return vetor;    
}


//    //Função que desfaz o processo de vetorização, gerando novamente os blocos 8x8 em formato de matriz.
double **matricizacao_bloco8x8(int *vetor) {
    if(vetor == NULL) return NULL;

    int index_matrix[8][8] = {
    {0, 1, 5 ,6, 14, 15, 27, 28},
    {2, 4, 7, 13, 16, 26, 29, 42},
    {3, 8, 12, 17, 25, 30, 41, 43},
    {9, 11, 18, 24, 31, 40, 44, 53},
    {10, 19, 23, 32, 39, 45, 52, 54},
    {20, 22, 33, 38, 46, 51, 55, 60},
    {21, 34, 37, 47, 50, 56, 59, 61},
    {35, 36, 48, 49, 57, 58, 62, 63}
    };

    double **bloco8x8 = (double **) aloca_matrix(1, 8, 8);
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            bloco8x8[i][j] = (double) vetor[index_matrix[i][j]];

    free(vetor);
    return bloco8x8;    
}


int ***vetorizacao(double ****blocos8x8, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos8x8 == NULL) return NULL;

    int ***vetores = (int ***) malloc(3*sizeof(int **));
    vetores[0] = (int **) malloc(qtd_blocos_y*sizeof(int *));
    vetores[1] = (int **) malloc(qtd_blocos_c*sizeof(int *));
    vetores[2] = (int **) malloc(qtd_blocos_c*sizeof(int *));

    for(int i = 0; i < qtd_blocos_y; i++)
        vetores[0][i] = vetorizacao_bloco8x8(blocos8x8[0][i]);

    for(int i = 0; i < qtd_blocos_c; i++) {
        vetores[1][i] = vetorizacao_bloco8x8(blocos8x8[1][i]);
        vetores[2][i] = vetorizacao_bloco8x8(blocos8x8[2][i]);
    }

    free(blocos8x8[0]); free(blocos8x8[1]); free(blocos8x8[2]);
    free(blocos8x8);

    return vetores;
}


double ****matricizacao(int ***vetor, int qtd_blocos_y, int qtd_blocos_c) {
    if(vetor == NULL) return NULL;

    double ****blocos8x8 = (double ****) malloc(3*sizeof(double ***));

    blocos8x8[0] = (double ***) malloc(qtd_blocos_y*sizeof(double **));
    blocos8x8[1] = (double ***) malloc(qtd_blocos_c*sizeof(double **));
    blocos8x8[2] = (double ***) malloc(qtd_blocos_c*sizeof(double **));

    for(int i = 0; i < qtd_blocos_y; i++)
        blocos8x8[0][i] = matricizacao_bloco8x8(vetor[0][i]);

    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos8x8[1][i] = matricizacao_bloco8x8(vetor[1][i]);
        blocos8x8[2][i] = matricizacao_bloco8x8(vetor[2][i]);
    }

    free(vetor[0]); free(vetor[1]); free(vetor[2]);
    free(vetor);

    return blocos8x8;
}


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
        
        // Coloca o valor nao zero
        if(pos < 64) {
            vetor[pos] = amplitude;
            pos++;
        }
    }
    
    return vetor;
}


Bloco_RLE ***codificar_rle(int ***vetores, int qtd_blocos_y, int qtd_blocos_c) {
    if(vetores == NULL) return NULL;
    
    Bloco_RLE ***blocos_rle = (Bloco_RLE ***) malloc(3 * sizeof(Bloco_RLE **));
    
    // Codifica blocos Y
    blocos_rle[0] = (Bloco_RLE **) malloc(qtd_blocos_y * sizeof(Bloco_RLE *));
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos_rle[0][i] = codificar_bloco_rle(vetores[0][i]);
    }
    
    // Codifica blocos Cb
    blocos_rle[1] = (Bloco_RLE **) malloc(qtd_blocos_c * sizeof(Bloco_RLE *));
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_rle[1][i] = codificar_bloco_rle(vetores[1][i]);
    }
    
    // Codifica blocos Cr
    blocos_rle[2] = (Bloco_RLE **) malloc(qtd_blocos_c * sizeof(Bloco_RLE *));
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_rle[2][i] = codificar_bloco_rle(vetores[2][i]);
    }
    
    free(vetores[0]); 
    free(vetores[1]); 
    free(vetores[2]);
    free(vetores);
    
    return blocos_rle;
}


int ***decodificar_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_rle == NULL) return NULL;
    
    int ***vetores = (int ***) malloc(3 * sizeof(int **));
    
    // Decodifica blocos Y
    vetores[0] = (int **) malloc(qtd_blocos_y * sizeof(int *));
    for(int i = 0; i < qtd_blocos_y; i++) {
        vetores[0][i] = decodificar_bloco_rle(blocos_rle[0][i]);
    }
    
    // Decodifica blocos Cb
    vetores[1] = (int **) malloc(qtd_blocos_c * sizeof(int *));
    for(int i = 0; i < qtd_blocos_c; i++) {
        vetores[1][i] = decodificar_bloco_rle(blocos_rle[1][i]);
    }
    
    // Decodifica blocos Cr
    vetores[2] = (int **) malloc(qtd_blocos_c * sizeof(int *));
    for(int i = 0; i < qtd_blocos_c; i++) {
        vetores[2][i] = decodificar_bloco_rle(blocos_rle[2][i]);
    }
    
    return vetores;
}

// Libera memoria de um bloco RLE
void liberar_bloco_rle(Bloco_RLE *bloco_rle) {
    if(bloco_rle != NULL) {
        if(bloco_rle->pares_ac != NULL) {
            free(bloco_rle->pares_ac);
        }
        free(bloco_rle);
    }
}


void liberar_blocos_rle(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_rle == NULL) return;
    
    // Libera blocos Y
    for(int i = 0; i < qtd_blocos_y; i++) {
        liberar_bloco_rle(blocos_rle[0][i]);
    }
    free(blocos_rle[0]);
    
    // Libera blocos Cb e Cr
    for(int i = 0; i < qtd_blocos_c; i++) {
        liberar_bloco_rle(blocos_rle[1][i]);
        liberar_bloco_rle(blocos_rle[2][i]);
    }
    free(blocos_rle[1]);
    free(blocos_rle[2]);
    
    free(blocos_rle);
}


// // Funcao de debug para imprimir informacoes do RLE
// void imprimir_debug_rle(Bloco_RLE *bloco_rle) {
//     if(bloco_rle == NULL) {
//         printf("Bloco RLE eh NULL\n");
//         return;
//     }
    
//     printf("DC: %d\n", bloco_rle->dc);
//     printf("Numero de pares AC: %d\n", bloco_rle->num_pares);
    
//     for(int i = 0; i < bloco_rle->num_pares; i++) {
//         printf("Par %d: (%d, %d)\n", i, 
//                bloco_rle->pares_ac[i].zeros_consecutivos, 
//                bloco_rle->pares_ac[i].amplitude);
//     }
//     printf("\n");
// }



// Funcao auxiliar para estimar bits de compressao
// int estimar_bits_huffman(Bloco_RLE *bloco_rle) {
//     int total_bits = 0;
    
//     // DC: categoria + amplitude (estimativa)
//     int dc_categoria = (bloco_rle->dc == 0) ? 0 : (int)(log2(abs(bloco_rle->dc)) + 1);
//     total_bits += 3 + dc_categoria;  // 3 bits Huffman + categoria bits amplitude
    
//     // ACs: estimativa baseada em zeros e amplitudes
//     for(int i = 0; i < bloco_rle->num_pares; i++) {
//         int zeros = bloco_rle->pares_ac[i].zeros_consecutivos;
//         int amplitude = bloco_rle->pares_ac[i].amplitude;
        
//         if(zeros == 0 && amplitude == 0) {
//             total_bits += 4;  // EOB
//         } else {
//             int ac_categoria = (amplitude == 0) ? 0 : (int)(log2(abs(amplitude)) + 1);
//             total_bits += 8 + ac_categoria;  // 8 bits simbolo + amplitude
//         }
//     }
    
//     return total_bits;
// }

// Codifica um bloco usando Huffman
Bloco_Huffman *codificar_bloco_huffman(Bloco_RLE *bloco_rle, int dc_anterior) {
    if(bloco_rle == NULL) return NULL;
    
    Bloco_Huffman *bloco_huff = (Bloco_Huffman *) malloc(sizeof(Bloco_Huffman));
    
    // DPCM para DC
    bloco_huff->diferenca_dc = bloco_rle->dc - dc_anterior;
    
    //bloco_huff->total_bits = estimar_bits_huffman(bloco_rle);
    
    // Armazenar dados originais
    bloco_huff->dados_originais = (Bloco_RLE *) malloc(sizeof(Bloco_RLE));
    bloco_huff->dados_originais->dc = bloco_rle->dc;
    bloco_huff->dados_originais->num_pares = bloco_rle->num_pares;
    bloco_huff->dados_originais->pares_ac = (Par_RLE *) malloc(bloco_rle->num_pares * sizeof(Par_RLE));
    
    for(int i = 0; i < bloco_rle->num_pares; i++) {
        bloco_huff->dados_originais->pares_ac[i] = bloco_rle->pares_ac[i];
    }
    
    return bloco_huff;
}

// Decodifica um bloco Huffman de volta para RLE
Bloco_RLE *decodificar_bloco_huffman(Bloco_Huffman *bloco_huff, int *dc_anterior) {
    if(bloco_huff == NULL) return NULL;
    
    Bloco_RLE *bloco_rle = (Bloco_RLE *) malloc(sizeof(Bloco_RLE));
    
    // Atualizar DC usando DPCM
    *dc_anterior += bloco_huff->diferenca_dc;
    
    // Copiar dados originais armazenados
    bloco_rle->dc = bloco_huff->dados_originais->dc;
    bloco_rle->num_pares = bloco_huff->dados_originais->num_pares;
    bloco_rle->pares_ac = (Par_RLE *) malloc(bloco_rle->num_pares * sizeof(Par_RLE));
    
    for(int i = 0; i < bloco_rle->num_pares; i++) {
        bloco_rle->pares_ac[i] = bloco_huff->dados_originais->pares_ac[i];
    }
    
    return bloco_rle;
}

// Codifica todos os blocos usando Huffman
Bloco_Huffman ***codificar_huffman(Bloco_RLE ***blocos_rle, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_rle == NULL) return NULL;
    
    Bloco_Huffman ***blocos_huff = (Bloco_Huffman ***) malloc(3 * sizeof(Bloco_Huffman **));
    
    // Codificar blocos Y
    blocos_huff[0] = (Bloco_Huffman **) malloc(qtd_blocos_y * sizeof(Bloco_Huffman *));
    int dc_anterior_y = 0;
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos_huff[0][i] = codificar_bloco_huffman(blocos_rle[0][i], dc_anterior_y);
        if(blocos_huff[0][i] != NULL) {
            dc_anterior_y += blocos_huff[0][i]->diferenca_dc;
        }
    }
    
    // Codificar blocos Cb
    blocos_huff[1] = (Bloco_Huffman **) malloc(qtd_blocos_c * sizeof(Bloco_Huffman *));
    int dc_anterior_cb = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_huff[1][i] = codificar_bloco_huffman(blocos_rle[1][i], dc_anterior_cb);
        if(blocos_huff[1][i] != NULL) {
            dc_anterior_cb += blocos_huff[1][i]->diferenca_dc;
        }
    }
    
    // Codificar blocos Cr
    blocos_huff[2] = (Bloco_Huffman **) malloc(qtd_blocos_c * sizeof(Bloco_Huffman *));
    int dc_anterior_cr = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_huff[2][i] = codificar_bloco_huffman(blocos_rle[2][i], dc_anterior_cr);
        if(blocos_huff[2][i] != NULL) {
            dc_anterior_cr += blocos_huff[2][i]->diferenca_dc;
        }
    }
    
    return blocos_huff;
}

// Decodifica todos os blocos Huffman
Bloco_RLE ***decodificar_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_huff == NULL) return NULL;
    
    Bloco_RLE ***blocos_rle = (Bloco_RLE ***) malloc(3 * sizeof(Bloco_RLE **));
    
    // Decodificar blocos Y
    blocos_rle[0] = (Bloco_RLE **) malloc(qtd_blocos_y * sizeof(Bloco_RLE *));
    int dc_anterior_y = 0;
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos_rle[0][i] = decodificar_bloco_huffman(blocos_huff[0][i], &dc_anterior_y);
    }
    
    // Decodificar blocos Cb
    blocos_rle[1] = (Bloco_RLE **) malloc(qtd_blocos_c * sizeof(Bloco_RLE *));
    int dc_anterior_cb = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_rle[1][i] = decodificar_bloco_huffman(blocos_huff[1][i], &dc_anterior_cb);
    }
    
    // Decodificar blocos Cr
    blocos_rle[2] = (Bloco_RLE **) malloc(qtd_blocos_c * sizeof(Bloco_RLE *));
    int dc_anterior_cr = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos_rle[2][i] = decodificar_bloco_huffman(blocos_huff[2][i], &dc_anterior_cr);
    }
    
    return blocos_rle;
}

// Libera memoria de um bloco Huffman
void liberar_bloco_huffman(Bloco_Huffman *bloco_huff) {
    if(bloco_huff != NULL) {
        if(bloco_huff->dados_originais != NULL) {
            liberar_bloco_rle(bloco_huff->dados_originais);
        }
        free(bloco_huff);
    }
}

// Libera todos os blocos Huffman
void liberar_blocos_huffman(Bloco_Huffman ***blocos_huff, int qtd_blocos_y, int qtd_blocos_c) {
    if(blocos_huff == NULL) return;
    
    // Liberar blocos Y
    for(int i = 0; i < qtd_blocos_y; i++) {
        liberar_bloco_huffman(blocos_huff[0][i]);
    }
    free(blocos_huff[0]);
    
    // Liberar blocos Cb e Cr
    for(int i = 0; i < qtd_blocos_c; i++) {
        liberar_bloco_huffman(blocos_huff[1][i]);
        liberar_bloco_huffman(blocos_huff[2][i]);
    }
    free(blocos_huff[1]);
    free(blocos_huff[2]);
    
    free(blocos_huff);
}


