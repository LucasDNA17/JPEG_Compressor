#ifndef BMP_COLOR_ELEMENTS_H
    #define BMP_COLOR_ELEMENTS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include "BMP_header.h"
    #include "matrix.h"
    #include <math.h>

    //Struct que armazena uma imagem BMP no formato RGB. Composta por três matrizes (R, G, B) 
    //e as dimensoes da imagem.
    typedef struct imagem_rgb {
        unsigned char **R;
        unsigned char **G;
        unsigned char **B;
        int Height;
        int Width;
    } Imagem_rgb;


    //Struct que armazena uma imagem BMP no formato YCbCr. Composta por três matrizes (Y, Cb, Cr), 
    // as dimensões da imagem, tanto no canal Y ("Height", "Width") quanto nos canais Cb e Cr ("Height_c", "Width_c").
    //Como, durante o processo de compressão/descompressão, as dimensões dos canais de lumiância e crominância mudam de
    //maneira diferente, é preciso separar as informações de dimensão.
    typedef struct imagem_ycbcr {
        double **Y;
        double **Cb;
        double **Cr;
        int Height;
        int Width;
        int Height_c;
        int Width_c;
    } Imagem_ycbcr;


    /* ----- Funções de leitura/escrita de imagem ----- */

    //Função que lê uma imagem BMP no formato RGB e retorna uma struct desse formato.
    //Entrada: ponteiro para o arquivo BMP; altura e comprimento da imagem
    //Saída: ponteiro para struct Imagem_rgb que armazena a imagem.   
    Imagem_rgb *getImage(FILE *F, int Height, int Width);

    //Função que escreve em um arquivo BMP a imagem no formato RGB.
    //Entrada: ponteiro para o arquivo BMP; ponteiro para a struct Imagem_rgb que contém a imagem.
    //Saída: nenhuma.
    void storeImage(FILE *F, Imagem_rgb *imagem);


    /* ----- Funções de conversão RGB/YCbCr ----- */
    
    //Função que "converte" uma struct de imagem RGB para uma no formato YCbCr.
    //Entrada: ponteiro para a struct Imagem_rgb.
    //Saída: ponteiro para a struct Imagem_Ycbcr que contém os pixels convertidos.
    Imagem_ycbcr *RGBtoYCbCr(Imagem_rgb *imagem);

    //Função que "converte" uma struct de imagem YCbCr para uma no formato RGB.
    //Entrada: ponteiro para a struct Imagem_ycbcr
    //Saída: ponteiro para a struct Imagem_rgb que contém os pixels convertidos.
    Imagem_rgb *YCbCrtoRGB(Imagem_ycbcr *imagem);


    /* ----- Funções de downsampling/upsampling ----- */

    //Função que realiza o downsampling 4:2:0 em uma imagem no formato YCbCr.
    //Entrada: ponteiro para a struct Imagem_ycbcr que contém a imagem.
    //Saída: nenhuma.
    void downsampling(Imagem_ycbcr *imagem);

    //Função que realiza o upsampling em uma imagem no formato YCbCr.
    //Entrada: ponteiro para a struct Imagem_ycbcr que contém a imagem.
    //Saída: nenhuma.
    void upsampling(Imagem_ycbcr *imagem);


    /* ----- Funções de levelshift ----- */

    //Função que realiza o downshift em uma imagem BMP, isto é, subtrai 128 do canal Y de cada pixel.
    //Entrada: ponteiro para struct da imagem no formato YCbCr.
    //Saída:nenhuma.
    void downLevelShift(Imagem_ycbcr *imagem);

    //Função que realiza o upshift em uma imagem BMP, isto é, adiciona 128 no canal Y de cada pixel.
    //Entrada: ponteiro para struct da imagem no formato YCbCr.
    //Saída:nenhuma.
    void upLevelShift(Imagem_ycbcr *imagem);


    /* ----- Funções de divisão da imagem em blocos 8x8 ----- */

    //Função que divide uma imagem BMP no formato YCbCr em blocos 8x8 de pixels.
    //Entrada: ponteiro para struct de imagem BMP no formato YCbCr.
    //Saída: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
    //da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
    //indexações representam o bloco 8x8 em si, no formato de matriz.
    double ****get_blocos8x8 (Imagem_ycbcr *imagem);

    //Função que cria uma struct de imagem BMP a partir de blocos 8x8.
    //Entrada: ponteiro quádruplo para double que armazena os blocos 8x8 (conforme especificado na sáida da função get_blocos8x8);
    //altura e largura da imagem no canal Y ("Height" e "Width"); altura e largura da imagem nos canais Cb e Cr ("Height_c" e "Width_c").
    //Saída: ponteiro para struct imagem BMP no formato YCbCr que armazena a imagem reconstruída.
    Imagem_ycbcr *blocos_to_imagem(double ****blocos, int Height, int Width, int Height_c, int Width_c);
    

#endif