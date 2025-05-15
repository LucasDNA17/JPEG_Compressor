#ifndef BMP_COLOR_ELEMENTS_H
    #define BMP_COLOR_ELEMENTS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include "BMP_header.h"
    #include "matrix.h"
    #include <math.h>


    typedef struct imagem_rgb {
        unsigned char **R;
        unsigned char **G;
        unsigned char **B;
        int Height;
        int Width;
    } Imagem_rgb;


    typedef struct imagem_ycbcr {
        double **Y;
        double **Cb;
        double **Cr;
        int Height;
        int Width;
        int Height_c;
        int Width_c;
    } Imagem_ycbcr;


    typedef struct rgb_pixel {
        unsigned char R;
        unsigned char G;
        unsigned char B;
    } RgbPixel;


    typedef struct ycbcr_pixel {
        double Y;
        double Cb;
        double Cr;
    } YcbcrPixel;




    Imagem_rgb *getImage(FILE *F, int Height, int Width);
    void storeImage(FILE *F, Imagem_rgb *imagem);
    Imagem_ycbcr *RGBtoYCbCr(Imagem_rgb *imagem);
    Imagem_rgb *YCbCrtoRGB(Imagem_ycbcr *imagem);
    void downsampling(Imagem_ycbcr *imagem);
    void upsampling(Imagem_ycbcr *imagem);
    void downLevelShift(Imagem_ycbcr *imagem);
    void upLevelShift(Imagem_ycbcr *imagem);
    int *get_indices_blocos(int Height, int Width);
    double ****get_blocos8x8 (Imagem_ycbcr *imagem);
    Imagem_ycbcr *blocos_to_imagem(double ****blocos, int Height, int Width, int Height_c, int Width_c);
    

#endif