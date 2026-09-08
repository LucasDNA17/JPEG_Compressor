#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "jpeg/jpeg.h"
#include "Compressao/Compressao.h"
#include "BMP_header/BMP_header.h"
#include "BMP_ColorElements/BMP_ColorElements.h"

void estatisticas_detalhadas(const char *arquivo_original, const char *arquivo_comprimido) {
    FILE *orig = fopen(arquivo_original, "rb");
    FILE *comp = fopen(arquivo_comprimido, "rb");
    
    if(orig == NULL || comp == NULL) {
        printf("Erro ao abrir arquivos para análise\n");
        if(orig) fclose(orig);
        if(comp) fclose(comp);
        return;
    }
    
    // Obter informações do arquivo original
    BMPFILEHEADER fileheader;
    BMPINFOHEADER infoheader;
    leituraFileHeader(orig, &fileheader);
    leituraInfoHeader(orig, &infoheader);
    
    // Obter tamanhos dos arquivos
    fseek(orig, 0, SEEK_END);
    long tamanho_original = ftell(orig);
    fseek(comp, 0, SEEK_END);
    long tamanho_comprimido = ftell(comp);
    
    fclose(orig);
    fclose(comp);
    
    // Calcular estatísticas
    double taxa_compressao = (double)tamanho_comprimido / tamanho_original;
    double economia = (1.0 - taxa_compressao) * 100.0;
    double fator_reducao = 1.0 / taxa_compressao;
    
    
    printf("\nESTATÍSTICAS DE COMPRESSÃO\n");
    printf("\nTamanhos dos arquivos:\n");
    printf("  Arquivo original: %ld bytes (%.2f KB)\n", tamanho_original, tamanho_original / 1024.0);
    printf("  Arquivo comprimido: %ld bytes (%.2f KB)\n", tamanho_comprimido, tamanho_comprimido / 1024.0);
    printf("  Diferença: %ld bytes\n", tamanho_original - tamanho_comprimido);
    printf("\nEficiência da compressão:\n");
    printf("  Taxa de compressão: %.2f%%\n", taxa_compressao * 100.0);
    printf("  Economia de espaço: %.2f%%\n", economia);
    printf("  Fator de redução: %.2fx\n", fator_reducao);
}

int main(void) {
    printf("SISTEMA DE COMPRESSÃO JPEG\n\n");
    
    char arquivo_entrada[256];
    char arquivo_comprimido[300];
    char arquivo_resultado[300];
    char nome_base[256];
    
    printf("Digite o nome do arquivo BMP de entrada: ");
    fgets(arquivo_entrada, sizeof(arquivo_entrada), stdin);
    
    // Remove a quebra de linha do fgets
    arquivo_entrada[strcspn(arquivo_entrada, "\n")] = 0;
    
    // Extrai o nome base
    strncpy(nome_base, arquivo_entrada, sizeof(nome_base) - 1);
    nome_base[sizeof(nome_base) - 1] = '\0';
    char *ponto = strrchr(nome_base, '.');
    if (ponto != NULL) {
        *ponto = '\0';
    }
    
    // Gera os nomes dos arquivos de saída
    snprintf(arquivo_comprimido, sizeof(arquivo_comprimido), "%s_comprimido.bin", nome_base);
    snprintf(arquivo_resultado, sizeof(arquivo_resultado), "%s_descomprimido.bmp", nome_base);
    
    // Verificar se a imagem é compatível
    if(!jpeg_verificar_compatibilidade(arquivo_entrada)) {
        printf("Erro: Imagem não é compatível com o algoritmo JPEG\n");
        return 1;
    }
    
    printf("\n");
    
    // Medir tempo de compressão
    clock_t inicio = clock();

    if(jpeg_comprime(arquivo_entrada, arquivo_comprimido)) {
        clock_t fim_compressao = clock();
        double tempo_compressao = ((double)(fim_compressao - inicio)) / CLOCKS_PER_SEC;
        
        //printf("\n");
        
        if(jpeg_descomprime(arquivo_comprimido, arquivo_resultado)) {
            clock_t fim_total = clock();
            double tempo_total = ((double)(fim_total - inicio)) / CLOCKS_PER_SEC;
            double tempo_descompressao = tempo_total - tempo_compressao;
            
            printf("\nArquivos criados:\n");
            printf("- Comprimido: %s\n", arquivo_comprimido);
            printf("- Descomprimido: %s\n", arquivo_resultado);
            
            // Estatísticas detalhadas
            estatisticas_detalhadas(arquivo_entrada, arquivo_comprimido);
            
            // Informações de tempo
            printf("\nINFORMAÇÕES DE TEMPO\n");
            printf("Tempo de compressão: %.3f segundos\n", tempo_compressao);
            printf("Tempo de descompressão: %.3f segundos\n", tempo_descompressao);
            printf("Tempo total: %.3f segundos\n", tempo_total);
            
        } else {
            printf("Erro durante a descompressão\n");
            return 1;
        }
    } else {
        printf("Erro durante a compressão\n");
        return 1;
    }
    
    printf("\nProcesso concluído com sucesso!\n");
    return 0;
}