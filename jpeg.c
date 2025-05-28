#include "jpeg.h"
#include "BMP_header.h"
#include "BMP_ColorElements.h"
#include "DCT.h"
#include "quant.h"
#include "CodEntropica.h"
#include "Compressao.h"
#include <stdio.h>
#include <stdlib.h>

int jpeg_comprime(const char* arquivo_bmp_entrada, const char* arquivo_bin_saida) {
    printf("=== COMPRESSÃO ===\n");

    
    // ETAPA 1: Verificação do arquivo de entrada
    FILE *arquivo = fopen(arquivo_bmp_entrada, "rb");
    if(arquivo == NULL) {
        printf("ERRO: Falha ao abrir arquivo '%s'\n", arquivo_bmp_entrada);
        return 0;
    }
    
    // ETAPA 2: Leitura dos cabeçalhos BMP
    BMPFILEHEADER fileheader;
    BMPINFOHEADER infoheader;
    
    leituraFileHeader(arquivo, &fileheader);
    leituraInfoHeader(arquivo, &infoheader);
    
    // Verificação da integridade dos cabeçalhos
    if(fileheader.bfType != 0x4D42) {
        printf("ERRO: Arquivo BMP inválido - assinatura incorreta\n");
        fclose(arquivo);
        return 0;
    }
    
    // ETAPA 3: Verificação de compatibilidade
    if(infoheader.biBitCount != 24) {
        printf("ERRO: Apenas BMPs 24-bit são suportados (encontrado: %d-bit)\n", 
               infoheader.biBitCount);
        fclose(arquivo);
        return 0;
    }
    
    if(infoheader.biCompression != 0) {
        printf("ERRO: Arquivo BMP não pode ter compressão (encontrado: %d)\n", 
               infoheader.biCompression);
        fclose(arquivo);
        return 0;
    }
    
    
    if(infoheader.biWidth < 8 || infoheader.biHeight < 8 || 
       infoheader.biWidth > 1280 || infoheader.biHeight > 800) {
        printf("ERRO: Dimensões devem estar entre 8x8 e 1280x800 (encontrado: %dx%d)\n", 
               infoheader.biWidth, infoheader.biHeight);
        fclose(arquivo);
        return 0;
    }
    
    
    // ETAPA 4: EXECUÇÃO DO PIPELINE DE COMPRESSÃO
    Imagem_rgb *imagem_rgb = getImage(arquivo, infoheader.biHeight, infoheader.biWidth);
    fclose(arquivo);
    
    if(imagem_rgb == NULL) {
        printf("ERRO: Falha no carregamento dos dados da imagem\n");
        return 0;
    }
    
    Imagem_ycbcr *imagem_ycbcr = RGBtoYCbCr(imagem_rgb);
    
    downsampling(imagem_ycbcr);
    
    downLevelShift(imagem_ycbcr);
    
    double ****blocos8x8 = get_blocos8x8(imagem_ycbcr);
    
    // Cálculo da quantidade de blocos baseado nas dimensões reais
    int qtd_blocos_y = (infoheader.biHeight/8) * (infoheader.biWidth/8);
    int qtd_blocos_c = (imagem_ycbcr->Height_c/8) * (imagem_ycbcr->Width_c/8);
    
    DCT(blocos8x8, qtd_blocos_y, qtd_blocos_c);
    
    quantizacao(blocos8x8, qtd_blocos_y, qtd_blocos_c);

    int ***vetores = vetorizacao(blocos8x8, qtd_blocos_y, qtd_blocos_c);
    
    Bloco_RLE ***blocos_rle = codificar_rle(vetores, qtd_blocos_y, qtd_blocos_c);
    
    Bloco_Huffman ***blocos_huffman = codificar_huffman(blocos_rle, qtd_blocos_y, qtd_blocos_c);
    
    int sucesso = salvar_imagem_comprimida(arquivo_bin_saida, blocos_huffman,
                                          infoheader.biWidth, infoheader.biHeight,
                                          qtd_blocos_y, qtd_blocos_c);
    
    if(sucesso) {
        printf("COMPRESSÃO EXECUTADA COM SUCESSO\n");
        printf("Arquivo de saída: %s\n", arquivo_bin_saida);
        return 1;
    } else {
        printf("ERRO: Falha na gravação do arquivo comprimido\n");
        return 0;
    }
}

int jpeg_descomprime(const char* arquivo_bin_entrada, const char* arquivo_bmp_saida) {
    printf("=== DESCOMPRESSÃO ===\n");
    printf("Arquivo de entrada: %s\n", arquivo_bin_entrada);
    printf("Arquivo de saída: %s\n\n", arquivo_bmp_saida);
    
    // Execução do pipeline completo de descompressão
    int sucesso = descompressao_completa_bin_para_bmp(arquivo_bin_entrada, arquivo_bmp_saida);
    
    if(sucesso) {
        printf("DESCOMPRESSÃO EXECUTADA COM SUCESSO\n");
        printf("Arquivo de saída: %s\n", arquivo_bmp_saida);
        return 1;
    } else {
        printf("ERRO: Falha no processo de descompressão\n");
        return 0;
    }
}

void jpeg_estatisticas(const char* arquivo_original, const char* arquivo_comprimido) {
    calcular_estatisticas_compressao(arquivo_original, arquivo_comprimido);
}

int jpeg_verificar_compatibilidade(const char* arquivo_bmp) {
    FILE *arquivo = fopen(arquivo_bmp, "rb");
    if(arquivo == NULL) {
        printf("ERRO: Arquivo não encontrado ou inacessível para leitura\n");
        return 0;
    }
    
    BMPFILEHEADER fileheader;
    BMPINFOHEADER infoheader;
    
    // Ler cabeçalhos
    leituraFileHeader(arquivo, &fileheader);
    leituraInfoHeader(arquivo, &infoheader);
    
    fclose(arquivo);
    
    // Verificação da assinatura BMP
    if(fileheader.bfType != 0x4D42) {
        printf("ERRO: Arquivo BMP inválido - assinatura: 0x%04X\n", fileheader.bfType);
        return 0;
    }
    
    // Verificação da profundidade de cor
    if(infoheader.biBitCount != 24) {
        printf("ERRO: Apenas arquivos BMP 24-bit são suportados\n");
        printf("Profundidade encontrada: %d-bit\n", infoheader.biBitCount);
        return 0;
    }
    
    // Verificação de compressão
    if(infoheader.biCompression != 0) {
        printf("ERRO: Arquivo BMP não pode conter compressão\n");
        printf("Tipo de compressão encontrado: %d\n", infoheader.biCompression);
        return 0;
    }
    
    // Verificação das dimensões
    if(infoheader.biWidth <= 0 || infoheader.biHeight <= 0) {
        printf("ERRO: Dimensões inválidas - %dx%d\n", infoheader.biWidth, infoheader.biHeight);
        return 0;
    }
    
    // Verificação se dimensões são múltiplas de 8 
    if(infoheader.biWidth % 8 != 0 || infoheader.biHeight % 8 != 0) {
        printf("ERRO: Dimensões devem ser múltiplas de 8\n");
        printf("Dimensões encontradas: %dx%d\n", infoheader.biWidth, infoheader.biHeight);
        return 0;
    }
    
    // Verificação dos limites mínimos e máximos
    if(infoheader.biWidth < 8 || infoheader.biHeight < 8) {
        printf("ERRO: Dimensões mínimas são 8x8 pixels\n");
        printf("Dimensões encontradas: %dx%d\n", infoheader.biWidth, infoheader.biHeight);
        return 0;
    }
    
    if(infoheader.biWidth > 1280 || infoheader.biHeight > 800) {
        printf("ERRO: Dimensões máximas são 1280x800 pixels\n");
        printf("Dimensões encontradas: %dx%d\n", infoheader.biWidth, infoheader.biHeight);
        return 0;
    }
    
    return 1;
} 