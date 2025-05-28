#include "BMP_header.h"


/* ----- Funções de leitura/escrita no FileHeader ----- */


//Função que realiza a leitura, campo a campo, do FileHeader de um arquivo BMP e salva as informações em uma struct.
void leituraFileHeader(FILE *F, BMPFILEHEADER *H) {
    fseek(F, 0, SEEK_SET); 
    fread(&H->bfType,sizeof (unsigned short int),1,F);
    fread(&H->bfSize,sizeof (unsigned int),1,F);
    fread(&H->bfReserved1,sizeof (unsigned short int),1,F);
    fread(&H->bfReserved2,sizeof (unsigned short int),1,F);
    fread(&H->bfOffBits,sizeof (unsigned int),1,F);
}

//Função que realiza a escrita, campo a campo, do FileHeader contido em uma struct em um arquivo BMP.
void escritaFileHeader(FILE *F, BMPFILEHEADER *H) {
    fseek(F, 0, SEEK_SET);
    fwrite(&H->bfType,sizeof (unsigned short int),1,F);
    fwrite(&H->bfSize,sizeof (unsigned int),1,F);
    fwrite(&H->bfReserved1,sizeof (unsigned short int),1,F);
    fwrite(&H->bfReserved2,sizeof (unsigned short int),1,F);
    fwrite(&H->bfOffBits,sizeof (unsigned int),1,F);
}



/* ----- Funções de leitura/escrita no InfoHeader ----- */



//Função que realiza a leitura, campo a campo, do InfoHeader de um arquivo BMP e salva as informações em uma struct.
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

//Função que realiza a escrita, campo a campo, do InfoHeader contido em uma struct em um arquivo BMP.
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
