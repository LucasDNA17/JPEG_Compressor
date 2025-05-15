#include <stdio.h>
#include "BMP_header.h"
#include "BMP_ColorElements.h"
#include "DCT.h"
#include "quant.h"
#include "CodEntropica.h"
#include <math.h>


#define PI 3.14159265358979323846


void calcula_matrix_C(void) {
    double **C = (double **) aloca_matrix(1,8,8);
    for(int j = 0; j < 8; j++) {
        double raiz = sqrt(8);
        C[0][j] = 1/raiz;
    }

    for(int i = 1; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            double argumento = (2*j + 1)*i*(PI);
            argumento /= 16;
            C[i][j] = cos(argumento)/2;
        }
    }

    printaMatrix(C, 8, 8);
    desaloca_matrix((void **) C, 8, 8);
} 


void printa_vetor(int *vetor) {
    for(int i = 0; i < 64; i++)
        printf("%d ", vetor[i]);

    printf("\n");
}




int main(void) {
    
    
    FILE *file = fopen("praia.bmp", "rb+");
    if(file == NULL) {
        printf("Erro na abertura do arquivo!\n");
        return 1;
    }

    BMPFILEHEADER fileheader;
    BMPINFOHEADER infoheader;
    leituraFileHeader(file, &fileheader);
    leituraInfoHeader(file, &infoheader);

    Imagem_rgb *image = getImage(file, infoheader.biHeight, infoheader.biWidth);
    FILE *imagem = fopen("imagem.bmp", "wb+");

    Imagem_ycbcr *imagem_ycbcr = RGBtoYCbCr(image);
    downsampling(imagem_ycbcr);
    downLevelShift(imagem_ycbcr);

    int Height = imagem_ycbcr->Height;
    int Width = imagem_ycbcr->Width;
    int Height_c = imagem_ycbcr->Height_c;
    int Width_c = imagem_ycbcr->Width_c;
    int qtd_blocos_y = (Height/8)*(Width/8);
    int qtd_blocos_c = (Height_c/8)*(Width_c/8);

    double ****blocos8x8 =get_blocos8x8(imagem_ycbcr);
    DCT(blocos8x8, qtd_blocos_y, qtd_blocos_c);

    quantizacao(blocos8x8, qtd_blocos_y, qtd_blocos_c);
    int ***vetores = vetorizacao(blocos8x8, qtd_blocos_y, qtd_blocos_c);
    double ****blocos8x8_reconstruidos = matrizicacao(vetores, qtd_blocos_y, qtd_blocos_c);


    quantizacao_inversa(blocos8x8_reconstruidos, qtd_blocos_y, qtd_blocos_c);


    DCT_inversa(blocos8x8_reconstruidos, qtd_blocos_y, qtd_blocos_c);

    Imagem_ycbcr *imagem_reconstruida = blocos_to_imagem(blocos8x8_reconstruidos, Height, Width, Height_c, Width_c);
    
    upLevelShift(imagem_reconstruida);
    upsampling(imagem_reconstruida);


    Imagem_rgb *imagem_rgb = YCbCrtoRGB(imagem_reconstruida);
    

    escritaFileHeader(imagem, &fileheader);
    escritaInfoHeader(imagem, &infoheader);
    storeImage(imagem, imagem_rgb);
    

    fclose(file);
    fclose(imagem);
    



    
    
    
    /*
    //Teste da DCT atualizado    
    double A[8][8] =
    {
    {52, 55, 61, 66, 70, 61, 64, 73},
    {63, 59, 66, 90, 109, 85, 69, 72},
    {62, 59, 68, 113, 144, 104, 66, 73},
    {63, 58, 71, 122, 154, 106, 70, 69},
    {67, 61, 68, 104, 126, 88, 68, 70},
    {79, 65, 60, 70, 77, 68, 58, 75},
    {85, 71, 64, 59, 55, 61, 65, 83},
    {87, 79, 69, 68, 65, 76, 78, 94},
    };
    

    /*
    double A[8][8] = {
        {154, 123, 123, 123, 123, 123, 123, 136},
        {192, 180, 136, 154, 154, 154, 136, 110},
        {254, 198, 154, 154, 180, 154, 123, 123},
        {239, 180, 136, 180, 180, 166, 123, 123},
        {180, 154, 136, 167, 166, 149, 136, 110},
        {128, 136, 123, 136, 154, 180, 198, 154},
        {123, 105, 110, 149, 136, 136, 180, 166},
        {110, 136, 123, 123, 123, 136, 154, 136}
    };
    

    double Y[8][8] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {79, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
    };
    

    double **A_ = (double **) aloca_matrix(1, 8, 8);
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            A_[i][j] = A[i][j] - 128;

    
    A_ = DCT8x8(A_);
    divide_bloco(A_, Y);
    printaMatrix(A_, 8, 8);
    printf("\n\n");
    int *vetor = vetorizacao_bloco8x8(A_);
    A_ = matrizicacao_bloco8x8(vetor);
    printaMatrix(A_, 8, 8);
    printf("\n\n");
    //printa_vetor(vetor);
    

    */




 

    //calcula_matrix_C();


    //TESTE DA DCT
    
    /*
    double A[8][8] =
    {
    {52, 55, 61, 66, 70, 61, 64, 73},
    {63, 59, 66, 90, 109, 85, 69, 72},
    {62, 59, 68, 113, 144, 104, 66, 73},
    {63, 58, 71, 122, 154, 106, 70, 69},
    {67, 61, 68, 104, 126, 88, 68, 70},
    {79, 65, 60, 70, 77, 68, 58, 75},
    {85, 71, 64, 59, 55, 61, 65, 83},
    {87, 79, 69, 68, 65, 76, 78, 94},
    };

    double B[8][8] = {
        {172, 179, 188, 191, 196, 200, 204, 174},
        {188, 187, 190, 193, 199, 201, 178, 101},
        {189, 189, 196, 197, 199, 183, 117,  84},
        {186, 192, 197, 199, 189, 130,  85,  85},
        {198, 197, 199, 192, 149, 100, 100,  95},
        {195, 195, 193, 158, 108,  98,  96,  96},
        {195, 189, 171, 111, 111, 108, 104,  96},
        {192, 177, 124, 110, 113, 113, 108, 100}
    };


    double C[8][8] = { 
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255 } };


    double M[8][8] = {
        {  26,  -5,  -5,  -5,  -5,  -5,  -5,   8 },
        {  64,  52,   8,  26,  26,  26,   8, -18 },
        { 126,  70,  26,  26,  26,   8,  -5,  -5 },
        { 111,  52,   8,  52,  52,  38,  -5,  -5 },
        {  52,  26,   8,  39,  38,  21,   8,   8 },
        {   0,   8,  -5,   8,  26,  52,  70,  26 },
        {  -5, -23, -18,  21,   8,   8,  52,  38 },
        { -18,   8,  -5,  -5,  -5,   8,  26,   8 }
    };

    double resultado[8][8];
    DCT_8x8(M, resultado);

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            printf("%.10lf ",  resultado[i][j]);
        }
        printf("\n");
    }
    */
    
    



    return 0;
}