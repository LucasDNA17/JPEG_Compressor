#ifndef JPEG_H
#define JPEG_H


int jpeg_comprime(const char* arquivo_bmp_entrada, const char* arquivo_bin_saida);
int jpeg_descomprime(const char* arquivo_bin_entrada, const char* arquivo_bmp_saida);
int jpeg_verificar_compatibilidade(const char* arquivo_bmp);

#endif