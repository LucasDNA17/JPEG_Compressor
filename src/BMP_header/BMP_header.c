#include "BMP_header.h"


/* ----- Funções de leitura/escrita no FileHeader ----- */


//Função que lê todas as informações do FileHeader de um arquivo BMP e as salva em uma struct.
//Entrada: ponteiro para o arquivo BMP; struct FileHeader onde as informações serão salvas.
//Saída: Nenhuma.
void leituraFileHeader(FILE *F, BMPFILEHEADER *H) {
    fseek(F, 0, SEEK_SET); 
    fread(&H->bfType,sizeof (unsigned short int),1,F);
    fread(&H->bfSize,sizeof (unsigned int),1,F);
    fread(&H->bfReserved1,sizeof (unsigned short int),1,F);
    fread(&H->bfReserved2,sizeof (unsigned short int),1,F);
    fread(&H->bfOffBits,sizeof (unsigned int),1,F);
}


//Função que escreve todas as informações de uma struct de FileHeader em um arquivo BMP.
//Entrada: ponteiro para o arquivo BMP; struct FileHeader que contém as informações a serem salvas.
//Saída: nenhuma.
void escritaFileHeader(FILE *F, BMPFILEHEADER *H) {
    fseek(F, 0, SEEK_SET);
    fwrite(&H->bfType,sizeof (unsigned short int),1,F);
    fwrite(&H->bfSize,sizeof (unsigned int),1,F);
    fwrite(&H->bfReserved1,sizeof (unsigned short int),1,F);
    fwrite(&H->bfReserved2,sizeof (unsigned short int),1,F);
    fwrite(&H->bfOffBits,sizeof (unsigned int),1,F);
}


/* ----- Funções de leitura/escrita no InfoHeader ----- */


//Função que lê todas as informações do InfoHeader de um arquivo BMP e as salva em uma struct.
//Entrada: ponteiro para o arquivo BMP; struct InfoHeader onde as informações serão salvas.
//Saída: Nenhuma.
void leituraInfoHeader(FILE *F, BMPINFOHEADER *H) {
    fseek(F, BMP_FILEHEADER_SIZE, SEEK_SET); 
    fread(&H->biSize, sizeof(unsigned int), 1, F);
    fread(&H->biWidth, sizeof(int), 1, F);
    fread(&H->biHeight, sizeof(int), 1, F);
    fread(&H->biPlanes, sizeof(unsigned short), 1, F);
    fread(&H->biBitCount, sizeof(unsigned short), 1, F);
    fread(&H->biCompression, sizeof(unsigned int), 1, F);
    fread(&H->biSizeImage, sizeof(unsigned int), 1, F);
    fread(&H->biXPelsPerMeter, sizeof(int), 1, F);
    fread(&H->biYPelsPerMeter, sizeof(int), 1, F);
    fread(&H->biClrUsed, sizeof(unsigned int), 1, F);
    fread(&H->biClrImportant, sizeof(unsigned int), 1, F);
}


//Função que escreve todas as informações de uma struct de InfoHeader em um arquivo BMP.
//Entrada: ponteiro para o arquivo BMP; struct InfoHeader onde as informações serão salvas.
//Saída: Nenhuma.
void escritaInfoHeader(FILE *F, BMPINFOHEADER *H) {
    fseek(F, BMP_FILEHEADER_SIZE, SEEK_SET); 
    fwrite(&H->biSize, sizeof(unsigned int), 1, F);
    fwrite(&H->biWidth, sizeof(int), 1, F);
    fwrite(&H->biHeight, sizeof(int), 1, F);
    fwrite(&H->biPlanes, sizeof(unsigned short), 1, F);
    fwrite(&H->biBitCount, sizeof(unsigned short), 1, F);
    fwrite(&H->biCompression, sizeof(unsigned int), 1, F);
    fwrite(&H->biSizeImage, sizeof(unsigned int), 1, F);
    fwrite(&H->biXPelsPerMeter, sizeof(int), 1, F);
    fwrite(&H->biYPelsPerMeter, sizeof(int), 1, F);
    fwrite(&H->biClrUsed, sizeof(unsigned int), 1, F);
    fwrite(&H->biClrImportant, sizeof(unsigned int), 1, F);
}