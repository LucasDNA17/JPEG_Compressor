#ifndef BMP_HEADER_H
    #define BMP_HEADER_H

    #include <stdio.h>


    #define BMP_FILEHEADER_SIZE 14  //2 + 4 + 2 + 2 + 4
    #define BMP_INFOHEADER_SIZE 40  //4 + 4 + 4 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 4
    #define BMP_HEADER_SIZE 54 //14 + 40 

    typedef struct bmpfileheader{
        unsigned short bfType; /* Magic number for file */
        unsigned int bfSize; /* Size of file */
        unsigned short bfReserved1; /* Reserved */
        unsigned short bfReserved2; /* ... */
        unsigned int bfOffBits; /* Offset to bitmap data */
    }BMPFILEHEADER;

    typedef struct bmpinfoheader{
        unsigned int biSize; /* Size of info header */
        int biWidth; /* Width of image */
        int biHeight; /* Height of image */
        unsigned short biPlanes; /* Number of color planes */
        unsigned short biBitCount; /* Number of bits per pixel */   
        unsigned int biCompression; /* Type of compression to use */
        unsigned int biSizeImage; /* Size of image data */
        int biXPelsPerMeter; /* X pixels per meter */
        int biYPelsPerMeter; /* Y pixels per meter */
        unsigned int biClrUsed; /* Number of colors used */
        unsigned int biClrImportant; /* Number of important colors */
    }BMPINFOHEADER;


    void leituraFileHeader(FILE *F, BMPFILEHEADER *H);
    void escritaFileHeader(FILE *F, BMPFILEHEADER *H);

    void leituraInfoHeader(FILE *F, BMPINFOHEADER *H);
    void escritaInfoHeader(FILE *F, BMPINFOHEADER *H);    
    

#endif