#include "Compressao.h"
#include "BMP_header.h"
#include "BMP_ColorElements.h"
#include "DCT.h"
#include "quant.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Referências externas às tabelas Huffman
extern const uint8_t huffman_dc_len[12];
extern const uint16_t huffman_dc_code[12];
extern const uint8_t huffman_ac_len[256];
extern const uint16_t huffman_ac_code[256];


//Cria e inicializa um buffer de bits para armazenar dados comprimidos
//Parametros: capacidade_inicial - tamanho inicial do buffer em bytes
//Retorno: ponteiro para o buffer de bits criado
Buffer_Bits *criar_buffer_bits(int capacidade_inicial) {
    Buffer_Bits *buffer = (Buffer_Bits *) malloc(sizeof(Buffer_Bits));
    buffer->capacidade = capacidade_inicial;
    buffer->dados = (unsigned char *) calloc(capacidade_inicial, sizeof(unsigned char));
    buffer->tamanho_buffer = 0;
    buffer->bit_atual = 0;
    buffer->byte_leitura = 0;
    buffer->bit_leitura = 0;
    return buffer;
}

//Libera a memoria alocada para um buffer de bits
//Parametros: buffer - ponteiro para o buffer a ser liberado
//Retorno: void
void liberar_buffer_bits(Buffer_Bits *buffer) {
    if(buffer != NULL) {
        if(buffer->dados != NULL) {
            free(buffer->dados);
        }
        free(buffer);
    }
}

//Escreve um bit no buffer de bits
//Parametros: buffer - ponteiro para o buffer, bit - valor do bit (0 ou 1)
//Retorno: void
void escrever_bit(Buffer_Bits *buffer, int bit) {
    // Expandir buffer se necessário
    int byte_necessario = buffer->tamanho_buffer;
    if(byte_necessario >= buffer->capacidade) {
        buffer->capacidade *= 2;
        buffer->dados = (unsigned char *) realloc(buffer->dados, buffer->capacidade);
    }
    
    // Escrever o bit na posição atual
    if(bit) {
        buffer->dados[byte_necessario] |= (1 << (7 - buffer->bit_atual));
    }
    
    buffer->bit_atual++;
    if(buffer->bit_atual == 8) {
        buffer->bit_atual = 0;
        buffer->tamanho_buffer++;
    }
}

//Escreve multiplos bits no buffer
//Parametros: buffer - ponteiro para o buffer, valor - valor a ser escrito, num_bits - numero de bits a escrever
//Retorno: void
void escrever_bits(Buffer_Bits *buffer, int valor, int num_bits) {
    for(int i = num_bits - 1; i >= 0; i--) {
        int bit = (valor >> i) & 1;
        escrever_bit(buffer, bit);
    }
}

//Le um bit do buffer de bits
//Parametros: buffer - ponteiro para o buffer
//Retorno: valor do bit lido (0 ou 1)
int ler_bit(Buffer_Bits *buffer) {
    if(buffer->byte_leitura >= buffer->capacidade) {
        return 0; // EOF
    }
    
    int bit = (buffer->dados[buffer->byte_leitura] >> (7 - buffer->bit_leitura)) & 1;
    
    buffer->bit_leitura++;
    if(buffer->bit_leitura == 8) {
        buffer->bit_leitura = 0;
        buffer->byte_leitura++;
    }
    
    return bit;
}

//Le multiplos bits do buffer
//Parametros: buffer - ponteiro para o buffer, num_bits - numero de bits a ler
//Retorno: valor inteiro formado pelos bits lidos
int ler_bits(Buffer_Bits *buffer, int num_bits) {
    int valor = 0;
    for(int i = 0; i < num_bits; i++) {
        valor = (valor << 1) | ler_bit(buffer);
    }
    return valor;
}

//Reseta a posicao de leitura do buffer para o inicio
//Parametros: buffer - ponteiro para o buffer
//Retorno: void
void resetar_posicao_leitura(Buffer_Bits *buffer) {
    buffer->byte_leitura = 0;
    buffer->bit_leitura = 0;
}



//Calcula a categoria JPEG de um valor DC ou AC
//Parametros: valor - valor inteiro para calcular a categoria
//Retorno: categoria do valor (0-11)
int obter_categoria(int valor) {
    if(valor == 0) return 0;
    int abs_valor = abs(valor);
    return (int)(log2(abs_valor)) + 1;
}

//Mapeia par (zeros, categoria) para indice da tabela AC Huffman
//Parametros: zeros - numero de zeros consecutivos, categoria - categoria do valor
//Retorno: indice na tabela AC Huffman
static int obter_indice_ac(int zeros, int categoria) {
    if(zeros == 0 && categoria == 0) return 0;  // EOB (End of Block)
    if(zeros > 15) zeros = 15;  // Máximo de 15 zeros consecutivos
    if(categoria > 10) categoria = 10;  // Máximo categoria 10
    return (zeros << 4) | categoria;  // (zeros * 16) + categoria
}

//Decodifica categoria DC usando tabelas Huffman padrao JPEG
//Parametros: buffer - ponteiro para o buffer de bits
//Retorno: categoria DC decodificada
static int decodificar_categoria_dc_huffman(Buffer_Bits *buffer) {
    int codigo_acumulado = 0;
    int bits_lidos = 0;
    
    for(int tamanho = 1; tamanho <= 9; tamanho++) {
        // Ler mais um bit
        int bit = ler_bit(buffer);
        codigo_acumulado = (codigo_acumulado << 1) | bit;
        bits_lidos++;
        
        // Verificar todas as categorias com este tamanho
        for(int categoria = 0; categoria < 12; categoria++) {
            if(huffman_dc_len[categoria] == tamanho && 
               huffman_dc_code[categoria] == codigo_acumulado) {
                return categoria;
            }
        }
    }
    
    return 0;  // Se não encontrou, retorna categoria 0
}

//Decodifica simbolo AC usando tabelas Huffman padrao JPEG
//Parametros: buffer - ponteiro para o buffer de bits
//Retorno: indice na tabela AC correspondente ao simbolo decodificado
static int decodificar_simbolo_ac_huffman(Buffer_Bits *buffer) {
    int codigo_acumulado = 0;
    int bits_lidos = 0;
    
    for(int tamanho = 1; tamanho <= 16; tamanho++) {
        // Ler mais um bit
        int bit = ler_bit(buffer);
        codigo_acumulado = (codigo_acumulado << 1) | bit;
        bits_lidos++;
        
        // Verificar todas as entradas AC com este tamanho
        for(int indice = 0; indice < 256; indice++) {
            if(huffman_ac_len[indice] == tamanho && 
               huffman_ac_code[indice] == codigo_acumulado) {
                return indice;
            }
        }
    }
    
    return 0;  // Se não encontrou, retorna EOB (End of Block)
}

//Codifica valor DC usando tabelas Huffman e categoria/amplitude
//Parametros: buffer - ponteiro para o buffer de bits, valor - valor DC a ser codificado
//Retorno: void
void codificar_categoria_amplitude(Buffer_Bits *buffer, int valor) {
    int categoria = obter_categoria(valor);
    
    // Validar categoria (0-11 para DC)
    if(categoria > 11) categoria = 11;
    
    // Usar tabela Huffman DC para escrever código da categoria
    uint16_t codigo_categoria = huffman_dc_code[categoria];
    uint8_t tamanho_categoria = huffman_dc_len[categoria];
    escrever_bits(buffer, codigo_categoria, tamanho_categoria);
    
    if(categoria > 0) {
        // Codificar amplitude usando tamanho variável
        if(valor > 0) {
            escrever_bits(buffer, valor, categoria);
        } else {
            // Para valores negativos, usar complemento
            int amplitude_codigo = valor + (1 << categoria) - 1;
            escrever_bits(buffer, amplitude_codigo, categoria);
        }
    }
}

//Decodifica valor DC usando tabelas Huffman e categoria/amplitude
//Parametros: buffer - ponteiro para o buffer de bits
//Retorno: valor DC decodificado
int decodificar_categoria_amplitude(Buffer_Bits *buffer) {
    int categoria = decodificar_categoria_dc_huffman(buffer);
    
    if(categoria == 0) return 0;
    
    int codigo = ler_bits(buffer, categoria);
    int limite = 1 << (categoria - 1);
    
    if(codigo >= limite) {
        return codigo; // Valor positivo
    } else {
        return codigo - (1 << categoria) + 1; // Valor negativo
    }
}

//Codifica simbolo AC usando tabelas Huffman
//Parametros: buffer - ponteiro para o buffer, zeros - zeros consecutivos, amplitude - valor AC
//Retorno: void
void codificar_simbolo_ac(Buffer_Bits *buffer, int zeros, int amplitude) {
    int categoria = obter_categoria(amplitude);
    int indice_ac = obter_indice_ac(zeros, categoria);
    
    // Validar índice AC (0-255)
    if(indice_ac > 255) indice_ac = 255;
    
    // Usar tabela Huffman AC para escrever código do par (zeros, categoria)
    uint16_t codigo_ac = huffman_ac_code[indice_ac];
    uint8_t tamanho_ac = huffman_ac_len[indice_ac];
    
    // Só escrever se o tamanho não for 0 (entrada válida na tabela)
    if(tamanho_ac > 0) {
        escrever_bits(buffer, codigo_ac, tamanho_ac);
        
        // Escrever amplitude se categoria > 0
        if(categoria > 0) {
            if(amplitude > 0) {
                escrever_bits(buffer, amplitude, categoria);
            } else {
                // Para valores negativos, usar complemento
                int amplitude_codigo = amplitude + (1 << categoria) - 1;
                escrever_bits(buffer, amplitude_codigo, categoria);
            }
        }
    }
}

//Decodifica simbolo AC usando tabelas Huffman
//Parametros: buffer - ponteiro para o buffer, zeros - ponteiro para armazenar zeros, amplitude - ponteiro para amplitude
//Retorno: void
void decodificar_simbolo_ac(Buffer_Bits *buffer, int *zeros, int *amplitude) {
    // Decodificar usando tabela Huffman AC
    int indice_ac = decodificar_simbolo_ac_huffman(buffer);
    
    // Extrair zeros e categoria do índice
    *zeros = (indice_ac >> 4) & 0x0F;  // 4 bits superiores
    int categoria = indice_ac & 0x0F;   // 4 bits inferiores
    
    // Se categoria é 0, amplitude também é 0
    if(categoria == 0) {
        *amplitude = 0;
        return;
    }
    
    // Ler amplitude usando categoria
    int codigo = ler_bits(buffer, categoria);
    int limite = 1 << (categoria - 1);
    
    if(codigo >= limite) {
        *amplitude = codigo; // Valor positivo
    } else {
        *amplitude = codigo - (1 << categoria) + 1; // Valor negativo
    }
}



//Comprime um bloco usando codificacao Huffman
//Parametros: bloco_huff - bloco com dados RLE a ser comprimido
//Retorno: ponteiro para o bloco comprimido
Bloco_Comprimido *comprimir_bloco_huffman(Bloco_Huffman *bloco_huff) {
    if(bloco_huff == NULL) return NULL;
    
    Bloco_Comprimido *bloco_comp = (Bloco_Comprimido *) malloc(sizeof(Bloco_Comprimido));
    Buffer_Bits *buffer = criar_buffer_bits(64); // Buffer inicial
    
    // Codificar diferença DC
    bloco_comp->dc_diferenca = bloco_huff->diferenca_dc;
    codificar_categoria_amplitude(buffer, bloco_huff->diferenca_dc);
    
    // Codificar dados AC do bloco RLE original
    Bloco_RLE *rle_original = bloco_huff->dados_originais;
    bloco_comp->num_simbolos_ac = rle_original->num_pares;
    
    for(int i = 0; i < rle_original->num_pares; i++) {
        int zeros = rle_original->pares_ac[i].zeros_consecutivos;
        int amplitude = rle_original->pares_ac[i].amplitude;
        codificar_simbolo_ac(buffer, zeros, amplitude);
    }
    
    // Finalizar buffer se necessário (padding)
    while(buffer->bit_atual != 0) {
        escrever_bit(buffer, 0);
    }
    
    // Copiar dados para o bloco comprimido
    bloco_comp->tamanho_bits = buffer->tamanho_buffer;
    bloco_comp->dados_bits = (unsigned char *) malloc(bloco_comp->tamanho_bits);
    memcpy(bloco_comp->dados_bits, buffer->dados, bloco_comp->tamanho_bits);
    
    liberar_bloco_huffman(bloco_huff);
    liberar_buffer_bits(buffer);
    return bloco_comp;
}

//Descomprime um bloco usando decodificacao Huffman
//Parametros: bloco_comp - bloco comprimido, dc_anterior - valor DC do bloco anterior
//Retorno: ponteiro para o bloco Huffman descomprimido
Bloco_Huffman *descomprimir_bloco_huffman(Bloco_Comprimido *bloco_comp, int dc_anterior) {
    if(bloco_comp == NULL) return NULL;
    
    // Criar buffer a partir dos dados comprimidos
    Buffer_Bits *buffer = criar_buffer_bits(bloco_comp->tamanho_bits);
    memcpy(buffer->dados, bloco_comp->dados_bits, bloco_comp->tamanho_bits);
    buffer->capacidade = bloco_comp->tamanho_bits;
    resetar_posicao_leitura(buffer);
    
    // Criar bloco Huffman
    Bloco_Huffman *bloco_huff = (Bloco_Huffman *) malloc(sizeof(Bloco_Huffman));
    
    // Decodificar diferença DC
    bloco_huff->diferenca_dc = decodificar_categoria_amplitude(buffer);
    
    // Criar bloco RLE original
    bloco_huff->dados_originais = (Bloco_RLE *) malloc(sizeof(Bloco_RLE));
    bloco_huff->dados_originais->dc = dc_anterior + bloco_huff->diferenca_dc;
    bloco_huff->dados_originais->num_pares = bloco_comp->num_simbolos_ac;
    
    // Verificar se num_simbolos_ac é válido
    if(bloco_comp->num_simbolos_ac > 0 && bloco_comp->num_simbolos_ac <= 63) {
        bloco_huff->dados_originais->pares_ac = (Par_RLE *) malloc(bloco_comp->num_simbolos_ac * sizeof(Par_RLE));
    } else {
        bloco_huff->dados_originais->pares_ac = NULL;
        bloco_huff->dados_originais->num_pares = 0;
    }
    
    // Decodificar símbolos AC apenas se os pares foram alocados
    if(bloco_huff->dados_originais->pares_ac != NULL) {
        for(int i = 0; i < bloco_comp->num_simbolos_ac; i++) {
            int zeros, amplitude;
            decodificar_simbolo_ac(buffer, &zeros, &amplitude);
            bloco_huff->dados_originais->pares_ac[i].zeros_consecutivos = zeros;
            bloco_huff->dados_originais->pares_ac[i].amplitude = amplitude;
        }
    }
      
    liberar_buffer_bits(buffer);
    return bloco_huff;
}

//Libera memoria alocada para um bloco comprimido
//Parametros: bloco_comp - ponteiro para o bloco comprimido
//Retorno: void
void liberar_bloco_comprimido(Bloco_Comprimido *bloco_comp) {
    if(bloco_comp != NULL) {
        if(bloco_comp->dados_bits != NULL) {
            free(bloco_comp->dados_bits);
        }
        free(bloco_comp);
    }
}



//Salva imagem comprimida em arquivo binario
//Parametros: nome_arquivo - nome do arquivo, blocos_huffman - blocos comprimidos, largura/altura - dimensoes originais, largura_c/altura_c - dimensoes com padding, qtd_blocos_y/c - quantidade de blocos por canal
//Retorno: 1 se sucesso, 0 se erro
int salvar_imagem_comprimida(const char *nome_arquivo, Bloco_Huffman ***blocos_huffman, int largura, int altura, int largura_c, int altura_c, int qtd_blocos_y, int qtd_blocos_c) {
    
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if(arquivo == NULL) return 0;
    
    // Escrever cabeçalho
    Cabecalho_Comprimido cabecalho;
    memcpy(cabecalho.assinatura, "BINC", 4);
    cabecalho.largura = largura;
    cabecalho.altura = altura;
    cabecalho.largura_c = largura_c;    // Nova: dimensão real com padding
    cabecalho.altura_c = altura_c;      // Nova: dimensão real com padding
    cabecalho.qtd_blocos_y = qtd_blocos_y;
    cabecalho.qtd_blocos_c = qtd_blocos_c;
    
    // Calcular tamanho total dos dados
    cabecalho.tamanho_dados = 0;
    fwrite(&cabecalho, sizeof(Cabecalho_Comprimido), 1, arquivo);
    
    int total_bytes = 0;
    
    // Comprimir e salvar blocos Y
    for(int i = 0; i < qtd_blocos_y; i++) {
        Bloco_Comprimido *comp = comprimir_bloco_huffman(blocos_huffman[0][i]);
        
        // Escrever metadados do bloco
        fwrite(&comp->dc_diferenca, sizeof(int), 1, arquivo);
        fwrite(&comp->num_simbolos_ac, sizeof(int), 1, arquivo);
        fwrite(&comp->tamanho_bits, sizeof(int), 1, arquivo);
        
        // Escrever dados comprimidos
        fwrite(comp->dados_bits, sizeof(unsigned char), comp->tamanho_bits, arquivo);
        
        total_bytes += sizeof(int) * 3 + comp->tamanho_bits;
        liberar_bloco_comprimido(comp);
    }

    free(blocos_huffman[0]);
    
    // Comprimir e salvar blocos Cb e Cr
    for(int canal = 1; canal <= 2; canal++) {
        for(int i = 0; i < qtd_blocos_c; i++) {
            Bloco_Comprimido *comp = comprimir_bloco_huffman(blocos_huffman[canal][i]);
            
            fwrite(&comp->dc_diferenca, sizeof(int), 1, arquivo);
            fwrite(&comp->num_simbolos_ac, sizeof(int), 1, arquivo);
            fwrite(&comp->tamanho_bits, sizeof(int), 1, arquivo);
            fwrite(comp->dados_bits, sizeof(unsigned char), comp->tamanho_bits, arquivo);
            
            total_bytes += sizeof(int) * 3 + comp->tamanho_bits;
            liberar_bloco_comprimido(comp);
        }

        free(blocos_huffman[canal]);
    }
    
    free(blocos_huffman);

    // Atualizar cabeçalho com tamanho real
    fseek(arquivo, 0, SEEK_SET);
    cabecalho.tamanho_dados = total_bytes;
    fwrite(&cabecalho, sizeof(Cabecalho_Comprimido), 1, arquivo);
    
    fclose(arquivo);
    return 1;
}

//Carrega imagem comprimida de arquivo binario
//Parametros: nome_arquivo - nome do arquivo, largura/altura - ponteiros para dimensoes originais, largura_c/altura_c - ponteiros para dimensoes com padding, qtd_blocos_y/c - ponteiros para quantidade de blocos
//Retorno: ponteiro para estrutura de blocos Huffman carregados
Bloco_Huffman ***carregar_imagem_comprimida(const char *nome_arquivo, int *largura, int *altura, int *largura_c, int *altura_c, int *qtd_blocos_y, int *qtd_blocos_c) {
    
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if(arquivo == NULL) return NULL;
    
    // Ler cabeçalho
    Cabecalho_Comprimido cabecalho;
    fread(&cabecalho, sizeof(Cabecalho_Comprimido), 1, arquivo);
    
    // Verificar assinatura
    if(strncmp(cabecalho.assinatura, "BINC", 4) != 0) {
        fclose(arquivo);
        return NULL;
    }
    
    *largura = cabecalho.largura;
    *altura = cabecalho.altura;
    *largura_c = cabecalho.largura_c;    // recuperar dimensão real
    *altura_c = cabecalho.altura_c;      // recuperar dimensão real
    *qtd_blocos_y = cabecalho.qtd_blocos_y;
    *qtd_blocos_c = cabecalho.qtd_blocos_c;
    
    // Alocar estrutura de blocos
    Bloco_Huffman ***blocos_huffman = (Bloco_Huffman ***) malloc(3 * sizeof(Bloco_Huffman **));
    blocos_huffman[0] = (Bloco_Huffman **) malloc(*qtd_blocos_y * sizeof(Bloco_Huffman *));
    blocos_huffman[1] = (Bloco_Huffman **) malloc(*qtd_blocos_c * sizeof(Bloco_Huffman *));
    blocos_huffman[2] = (Bloco_Huffman **) malloc(*qtd_blocos_c * sizeof(Bloco_Huffman *));
    
    
    // Carregar blocos Y
    int dc_anterior_y = 0;
    for(int i = 0; i < *qtd_blocos_y; i++) {
        Bloco_Comprimido comp;
        fread(&comp.dc_diferenca, sizeof(int), 1, arquivo);
        fread(&comp.num_simbolos_ac, sizeof(int), 1, arquivo);
        fread(&comp.tamanho_bits, sizeof(int), 1, arquivo);
        
        comp.dados_bits = (unsigned char *) malloc(comp.tamanho_bits);
        fread(comp.dados_bits, sizeof(unsigned char), comp.tamanho_bits, arquivo);
        
        blocos_huffman[0][i] = descomprimir_bloco_huffman(&comp, dc_anterior_y);
        dc_anterior_y += comp.dc_diferenca;
        
        free(comp.dados_bits);
    }
    
    
    // Carregar blocos Cb e Cr
    int dc_anterior_cb = 0, dc_anterior_cr = 0;
    int *dc_anterior[] = {&dc_anterior_cb, &dc_anterior_cr};
    
    for(int canal = 1; canal <= 2; canal++) {
        for(int i = 0; i < *qtd_blocos_c; i++) {
            Bloco_Comprimido comp;
            fread(&comp.dc_diferenca, sizeof(int), 1, arquivo);
            fread(&comp.num_simbolos_ac, sizeof(int), 1, arquivo);
            fread(&comp.tamanho_bits, sizeof(int), 1, arquivo);
            
            comp.dados_bits = (unsigned char *) malloc(comp.tamanho_bits);
            fread(comp.dados_bits, sizeof(unsigned char), comp.tamanho_bits, arquivo);
            
            blocos_huffman[canal][i] = descomprimir_bloco_huffman(&comp, *dc_anterior[canal-1]);
            *dc_anterior[canal-1] += comp.dc_diferenca;
            
            free(comp.dados_bits);
        }
    }
    
    fclose(arquivo);
    return blocos_huffman;
}





//Executa descompressao completa de arquivo binario para BMP
//Parametros: arquivo_bin - nome do arquivo binario comprimido, arquivo_bmp_saida - nome do arquivo BMP de saida
//Retorno: 1 se sucesso, 0 se erro
int descompressao_completa_bin_para_bmp(const char *arquivo_bin, const char *arquivo_bmp_saida) {
    
    
    // ETAPA 1: Carregar arquivo .bin
    int largura, altura, largura_c, altura_c, qtd_blocos_y, qtd_blocos_c;
    Bloco_Huffman ***blocos_huffman = carregar_imagem_comprimida(arquivo_bin, &largura, &altura, &largura_c, &altura_c, &qtd_blocos_y, &qtd_blocos_c);
    
    if(blocos_huffman == NULL) {
        printf("Erro ao carregar arquivo .bin\n");
        return 0;
    }
    
    
    // ETAPA 2: Huffman -> RLE
    Bloco_RLE ***blocos_rle = decodificar_huffman(blocos_huffman, qtd_blocos_y, qtd_blocos_c);
    
    // ETAPA 3: RLE -> Vetores
    int ***vetores = decodificar_rle(blocos_rle, qtd_blocos_y, qtd_blocos_c);
    
    // ETAPA 4: Vetores -> Matrizes 8x8 
    double ****blocos8x8 = matricizacao(vetores, qtd_blocos_y, qtd_blocos_c);
    
    // ETAPA 5: Quantização inversa
    quantizacao_inversa(blocos8x8, qtd_blocos_y, qtd_blocos_c);
    
    // ETAPA 6: DCT inversa
    DCT_inversa(blocos8x8, qtd_blocos_y, qtd_blocos_c);
    
    // ETAPA 7: Reconstruir imagem YCbCr usando dimensões corretas do arquivo
    Imagem_ycbcr *imagem_ycbcr = blocos_to_imagem(blocos8x8, altura, largura, altura_c, largura_c);
    
    // ETAPA 8: Level shift (+128)
    upLevelShift(imagem_ycbcr);
    
    // ETAPA 9: Upsampling (4:2:0 -> 4:4:4)
    upsampling(imagem_ycbcr);
    
    // ETAPA 10: YCbCr -> RGB
    Imagem_rgb *imagem_rgb = YCbCrtoRGB(imagem_ycbcr);
    
    // ETAPA 11: Salvar como BMP
    FILE *arquivo_saida = fopen(arquivo_bmp_saida, "wb");
    if(arquivo_saida == NULL) {
        printf("Erro ao criar arquivo de saída\n");
        return 0;
    }
    
    // Criar cabeçalhos BMP
    BMPFILEHEADER fileheader;
    BMPINFOHEADER infoheader;
    
    // Configurar cabeçalho do arquivo
    fileheader.bfType = 0x4D42;  // "BM"
    fileheader.bfSize = 54 + (largura * altura * 3);  // 54 = tamanho fixo cabeçalho BMP
    fileheader.bfReserved1 = 0;
    fileheader.bfReserved2 = 0;
    fileheader.bfOffBits = 54;  // Offset fixo padrão BMP
    
    // Configurar cabeçalho da imagem
    infoheader.biSize = 40;  // Tamanho fixo do BITMAPINFOHEADER
    infoheader.biWidth = largura;
    infoheader.biHeight = altura;
    infoheader.biPlanes = 1;
    infoheader.biBitCount = 24;
    infoheader.biCompression = 0;
    infoheader.biSizeImage = largura * altura * 3;
    infoheader.biXPelsPerMeter = 2835;  // Resolução padrão 72 DPI
    infoheader.biYPelsPerMeter = 2835;  // Resolução padrão 72 DPI
    infoheader.biClrUsed = 0;
    infoheader.biClrImportant = 0;
    
    // Escrever cabeçalhos
    escritaFileHeader(arquivo_saida, &fileheader);
    escritaInfoHeader(arquivo_saida, &infoheader);
    
    // Escrever dados da imagem
    storeImage(arquivo_saida, imagem_rgb);
    
    fclose(arquivo_saida);

    
    return 1;
} 