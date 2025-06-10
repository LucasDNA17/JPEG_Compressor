
#ifndef BMP_HEADER_H
    #define BMP_HEADER_H

    #include <stdio.h>


    //Tamanho do "File Header" do arquivo BMP
    #define BMP_FILEHEADER_SIZE 14  //3*sizeof(unsigned short) + 2*sizeof(unsigned int)

    //Tamanho do "Info Header" do arquivo bmp
    #define BMP_INFOHEADER_SIZE 40  //2*sizeof(unsigned short) + 5*sizeof(unsigned int) + 4*sizeof(int) 

    //Tamanho total do Header do arquivo VMP
    #define BMP_HEADER_SIZE 54 //BMP_FILEHEADER_SIZE + BMP_INFOHEADER_SIZE

    //Struct que armazena as informações do FileHeader de um arquivo BMP.
    typedef struct bmpfileheader{
        unsigned short bfType; /* Magic number for file */
        unsigned int bfSize; /* Size of file */
        unsigned short bfReserved1; 
        unsigned short bfReserved2;
        unsigned int bfOffBits; /* Offset to bitmap data */
    }BMPFILEHEADER;

    //Struct que armazena as informações do InfoHeader de um arquivo BMP.
    typedef struct bmpinfoheader{
        unsigned int biSize; /* Size of info header */
        int biWidth; /* Width of image */
        int biHeight; /* Height of image */
        unsigned short biPlanes; 
        unsigned short biBitCount; /* Number of bits per pixel */   
        unsigned int biCompression; /* Type of compression to use */
        unsigned int biSizeImage; /* Size of image data */
        int biXPelsPerMeter; /* X pixels per meter */
        int biYPelsPerMeter; /* Y pixels per meter */
        unsigned int biClrUsed; /* Number of colors used */
        unsigned int biClrImportant; /* Number of important colors */
    }BMPINFOHEADER;


    //Função que lê todas as informações do FileHeader de um arquivo BMP e as salva em uma struct.
    //Entrada: ponteiro para o arquivo BMP; struct FileHeader onde as informações serão salvas.
    //Saída: Nenhuma.
    void leituraFileHeader(FILE *F, BMPFILEHEADER *H);

    //Função que escreve todas as informações de uma struct de FileHeader em um arquivo BMP.
    //Entrada: ponteiro para o arquivo BMP; struct FileHeader que contém as informações a serem salvas.
    void escritaFileHeader(FILE *F, BMPFILEHEADER *H);


    //Função que lê todas as informações do InfoHeader de um arquivo BMP e as salva em uma struct.
    //Entrada: ponteiro para o arquivo BMP; struct InfoHeader onde as informações serão salvas.
    //Saída: Nenhuma.
    void leituraInfoHeader(FILE *F, BMPINFOHEADER *H);


    //Função que escreve todas as informações de uma struct de InfoHeader em um arquivo BMP.
    //Entrada: ponteiro para o arquivo BMP; struct InfoHeader onde as informações serão salvas.
    //Saída: Nenhuma.
    void escritaInfoHeader(FILE *F, BMPINFOHEADER *H);    
    

#endif