#include <stdio.h>
#include <string.h>
#include "jpeg.h"

int main(void) {
    printf("=== SISTEMA DE COMPRESSÃO JPEG ===\n\n");
    
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
    strncpy(arquivo_comprimido, nome_base, sizeof(arquivo_comprimido) - 1);
    arquivo_comprimido[sizeof(arquivo_comprimido) - 1] = '\0';
    strncat(arquivo_comprimido, "_comprimido.bin", sizeof(arquivo_comprimido) - strlen(arquivo_comprimido) - 1);
    
    strncpy(arquivo_resultado, nome_base, sizeof(arquivo_resultado) - 1);
    arquivo_resultado[sizeof(arquivo_resultado) - 1] = '\0';
    strncat(arquivo_resultado, "_descomprimido.bmp", sizeof(arquivo_resultado) - strlen(arquivo_resultado) - 1);
    
    // Verificar se a imagem é compatível
    if(!jpeg_verificar_compatibilidade(arquivo_entrada)) {
        printf("Erro: Imagem não é compatível com o algoritmo JPEG\n");
        return 1;
    }
    
    printf("\n");
    

    if(jpeg_comprime(arquivo_entrada, arquivo_comprimido)) {

        jpeg_estatisticas(arquivo_entrada, arquivo_comprimido);
        printf("\n");
        
        if(jpeg_descomprime(arquivo_comprimido, arquivo_resultado)) {
            printf("\nArquivos criados:\n");
            printf("- Comprimido: %s\n", arquivo_comprimido);
            printf("- Descomprimido: %s\n", arquivo_resultado);
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