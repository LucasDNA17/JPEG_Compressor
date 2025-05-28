#include "BMP_ColorElements.h"

#define max(a,b) ((a > b) ? (a) : (b))
#define min(a,b) ((a < b) ? (a) : (b))

void desaloca_imagem_rgb(Imagem_rgb *imagem) {
    if(imagem == NULL)
        return;

    desaloca_matrix((void **)imagem->R, imagem->Height, imagem->Width);
    desaloca_matrix((void **)imagem->G, imagem->Height, imagem->Width);
    desaloca_matrix((void **)imagem->B, imagem->Height, imagem->Width);
    free(imagem);
    imagem = NULL;
}


void desaloca_imagem_ycbcr(Imagem_ycbcr *imagem) {
    if(imagem == NULL)
        return;

    desaloca_matrix((void **)imagem->Y, imagem->Height, imagem->Width);
    desaloca_matrix((void **)imagem->Cb, imagem->Height_c, imagem->Width_c);
    desaloca_matrix((void **)imagem->Cr, imagem->Height_c, imagem->Width_c);
    free(imagem);
    imagem = NULL;
}


Imagem_rgb *aloca_imagem_rgb(int Height, int Width) {
    Imagem_rgb *imagem = (Imagem_rgb *) malloc(sizeof(Imagem_rgb));
    if(imagem == NULL) {
        printf("Erro de alocação de imagem!\n");
        return NULL;
    }

    imagem->Height = Height;
    imagem->Width = Width;
    imagem->R = (unsigned char **) aloca_matrix(0, imagem->Height, imagem->Width);
    imagem->G = (unsigned char **) aloca_matrix(0, imagem->Height, imagem->Width);
    imagem->B = (unsigned char **) aloca_matrix(0, imagem->Height, imagem->Width);

    return imagem; 
}



Imagem_ycbcr *aloca_imagem_ycbcr(int Height, int Width, int Height_c, int Width_c) {
    Imagem_ycbcr *imagem = (Imagem_ycbcr *) malloc(sizeof(Imagem_ycbcr));
    if(imagem == NULL) {
        printf("Erro de alocação de imagem!\n");
        return NULL;
    }

    imagem->Height = Height;
    imagem->Width = Width;
    imagem->Height_c = Height_c;
    imagem->Width_c = Width_c;
    imagem->Y = (double **) aloca_matrix(1, imagem->Height, imagem->Width);
    imagem->Cb = (double **) aloca_matrix(1,imagem->Height_c, imagem->Width_c);
    imagem->Cr = (double **) aloca_matrix(1,imagem->Height_c, imagem->Width_c);
    return imagem;
}


/*BGR - blue, green, red*/
Imagem_rgb *getImage(FILE *F, int Height, int Width) {
    Imagem_rgb *imagem = aloca_imagem_rgb(Height, Width);

    fseek(F, BMP_HEADER_SIZE, SEEK_SET);
    for(int i = 0; i < Height; i++)
        for(int j = 0; j < Width; j++) {
            fread(&((imagem->B)[i][j]), sizeof(unsigned char), 1, F);
            fread(&((imagem->G)[i][j]), sizeof(unsigned char), 1, F);
            fread(&((imagem->R)[i][j]), sizeof(unsigned char), 1, F);
        }

    return imagem;
}

void storeImage(FILE *F, Imagem_rgb *imagem) {
    for(int i = 0; i < imagem->Height; i++)
        for(int j = 0; j < imagem->Width; j++) {
            fwrite(&((imagem->B)[i][j]), sizeof(unsigned char), 1, F);
            fwrite(&((imagem->G)[i][j]), sizeof(unsigned char), 1, F);
            fwrite(&((imagem->R)[i][j]), sizeof(unsigned char), 1, F);
        }

    desaloca_imagem_rgb(imagem);
    return;
}


/*Y = 0.299R + 0.587G + 0.114B */
/*Cb = 0.564(B − Y ) */
/*Cr = 0.713(R − Y ) */
Imagem_ycbcr *RGBtoYCbCr(Imagem_rgb *imagem) {
    Imagem_ycbcr *converted_image = aloca_imagem_ycbcr(imagem->Height, imagem->Width, imagem->Height, imagem->Width);
   
    for(int i = 0; i < converted_image->Height; i++) {
        for(int j = 0; j < converted_image->Width; j++) {
            double R = (double) (imagem->R)[i][j];
            double G = (double) (imagem->G)[i][j];
            double B = (double) (imagem->B)[i][j];
            
            
            (converted_image->Y)[i][j] = min(max(0.299*R + 0.587*G + 0.114*B, 0), 255);
            (converted_image->Cb)[i][j] = min(max(128 - 0.168736*R - 0.331264*G + 0.5*B, 0), 255); 
            (converted_image->Cr)[i][j] = min(max(128 + 0.5*R - 0.418688*G - 0.081312*B, 0), 255);  
        }
    }

    desaloca_imagem_rgb(imagem);
    return converted_image;
}

/*
R = Y + 1.402Cr (4)
G = Y − 0.344Cb − 0.714Cr (5)
B = Y + 1.772Cb
*/ 
Imagem_rgb *YCbCrtoRGB(Imagem_ycbcr *imagem) {
    Imagem_rgb *converted_image = aloca_imagem_rgb(imagem->Height, imagem->Width);
    
    for(int i = 0; i < imagem->Height; i++)
        for(int j = 0; j < imagem->Width; j++) {
            
            double Cb_shifted = (imagem->Cb)[i][j] - 128;
            double Cr_shifted = (imagem->Cr)[i][j] - 128;
            
            double R = (imagem->Y)[i][j] + 1.402 * Cr_shifted;
            double G = (imagem->Y)[i][j] - 0.344136 * Cb_shifted - 0.714136 * Cr_shifted;
            double B = (imagem->Y)[i][j] + 1.772 * Cb_shifted;

            (converted_image->R)[i][j] = min(max(round(R), 0), 255);
            (converted_image->G)[i][j] = min(max(round(G), 0), 255);
            (converted_image->B)[i][j] = min(max(round(B), 0), 255);
        }

    desaloca_imagem_ycbcr(imagem);
    return converted_image;
}



int comp_f(const void *a, const void *b) {
    return *((double *) a) > *((double *) b)  ? 1 : 0;
}


/*O tamanho das matrizes de crominância permanecem idênticos ou são reduzidos pela metade horizontal e verticalmente?*/
/*--> Possível problema com a DCT, que espera blocos 8x8*/
void downsampling(Imagem_ycbcr *imagem) {
    if(imagem == NULL)
        return;

    int tam_vertical = imagem->Height;
    int tam_horizontal = imagem->Width;

     //Correção do tamanho
    
    tam_vertical /= 2;
    tam_horizontal  /= 2;

    if(tam_vertical % 8 != 0)
        tam_vertical += 8 - (tam_vertical % 8);

    if(tam_horizontal % 8 != 0)
        tam_horizontal += 8 - (tam_horizontal % 8);
    
    imagem->Height_c = tam_vertical;
    imagem->Width_c = tam_horizontal;


    double **matrix_Cb = (double **) aloca_matrix(1, tam_vertical, tam_horizontal);
    double **matrix_Cr = (double **) aloca_matrix(1, tam_vertical, tam_horizontal);
    

    int contador_fileira = 0;
    int contador_coluna = 0;
    for(int i = 0; i < imagem->Height; i+= 2) {
        contador_coluna = 0;
        for(int j = 0; j < imagem->Width; j+= 2) {
            double a = (imagem->Cb)[i][j];
            double b = (imagem->Cb)[i][j + 1];
            double c = (imagem->Cb)[i + 1][j];
            double d = (imagem->Cb)[i + 1][j + 1];

            double a0 = (imagem->Cr)[i][j];
            double b0 = (imagem->Cr)[i][j + 1];
            double c0 = (imagem->Cr)[i + 1][j];
            double d0 = (imagem->Cr)[i + 1][j + 1];

            double mediab = (a + b + c + d)/4;
            mediab = min(max(mediab, 0), 255);
            double mediar = (a0 + b0 + c0 + d0)/4;
            mediar = min(max(mediar, 0), 255);
 
            matrix_Cb[contador_fileira][contador_coluna] = mediab;
            matrix_Cr[contador_fileira][contador_coluna] = mediar;

            contador_coluna++;

        }
        contador_fileira++;
    }

    //Preenche os espaços restantes com as colunas ou fileiras anteriores
    
    for(int i = 0; i < imagem->Height/2; i++)
        for(int j = imagem->Width/2; j < tam_horizontal; j++) {
            matrix_Cb[i][j] = matrix_Cb[i][j - 1];
            matrix_Cr[i][j] = matrix_Cr[i][j - 1];
        }

    for(int i = imagem->Height/2; i < tam_vertical; i++)
        for(int j = 0; j < tam_horizontal; j++) {
            matrix_Cb[i][j] = matrix_Cb[i - 1][j];
            matrix_Cr[i][j] = matrix_Cr[i - 1][j];
        }

    
    desaloca_matrix((void **)imagem->Cb, imagem->Height, imagem->Width);
    desaloca_matrix((void **)imagem->Cr, imagem->Height, imagem->Width);

    imagem->Cb = matrix_Cb;
    imagem->Cr = matrix_Cr;
}





void upsampling(Imagem_ycbcr *imagem) {
    if(imagem == NULL)
        return;

    double **matrix_Cb = (double **) aloca_matrix(1, imagem->Height, imagem->Width);
    double **matrix_Cr = (double **) aloca_matrix(1, imagem->Height, imagem->Width);

    int fileira = 0, coluna = 0;
    for(int i = 0; i < imagem->Height; i += 2) {
        coluna = 0;
        for(int j = 0; j < imagem->Width; j += 2) {
            matrix_Cr[i][j] = matrix_Cr[i + 1][j] = matrix_Cr[i][j + 1] = matrix_Cr[i +1][j + 1] = (imagem->Cr)[fileira][coluna];
            matrix_Cb[i][j] = matrix_Cb[i + 1][j] = matrix_Cb[i][j + 1] = matrix_Cb[i +1][j + 1] = (imagem->Cb)[fileira][coluna];
            coluna++;
        }
        fileira++;
    }

    desaloca_matrix((void **)imagem->Cb, (imagem->Height_c), (imagem->Width_c));
    desaloca_matrix((void **)imagem->Cr, (imagem->Height_c), (imagem->Width_c));


    imagem->Height_c = imagem->Height;
    imagem->Width_c = imagem->Width;
    imagem->Cb = matrix_Cb;
    imagem->Cr = matrix_Cr;  
}


void downLevelShift(Imagem_ycbcr *imagem) {
    for(int i = 0; i < imagem->Height; i++) {
        for(int j = 0; j < imagem->Width; j++)
            (imagem->Y)[i][j] -= 128;
    }
    
    // Cb e Cr já estão na faixa correta [0, 255], não precisam de level shift
}

void upLevelShift(Imagem_ycbcr *imagem) {
    for(int i = 0; i < imagem->Height; i++) {
        for(int j = 0; j < imagem->Width; j++)
            (imagem->Y)[i][j] += 128;
    }
    
    // Cb e Cr já estão na faixa correta [0, 255], não precisam de level shift reverso
}


/*
//Cada par k_i, k_(i + 1) é a coordenada da matrix do canto esquerdo do bloco 8x8 
int *get_indices_blocos(int Height, int Width) {
    if(Height % 8 != 0 || Width % 8 != 0)
        exit(1);

    int *indices = (int *) malloc(2*((int)Height/8)*((int)Width/8)*sizeof(int));

    if(indices == NULL) {
        return NULL;
    }

    int ind_fileira = 0;
    int ind_coluna = 0;
    for(int i = 0; i < 2*(Height/8)*(Width/8) - 1; i += 2) {
        indices[i] = ind_fileira;
        indices[i + 1] = ind_coluna;
        
        ind_coluna += 8;
        if(ind_coluna >= Width) {
            ind_coluna = 0;
            ind_fileira += 8;
        } 
    }


    return indices;
}
*/


int *get_indices_blocos(int Height, int Width) {
    if(Height % 8 != 0 || Width % 8 != 0)
        exit(1);

    int *indices = (int *) malloc(2*((int)Height/8)*((int)Width/8)*sizeof(int));
    if(indices == NULL) {
        printf("Erro na alocação do vetor de índices!\n");
        return NULL;
    }

    if(indices == NULL) {
        return NULL;
    }

    int ind_fileira = 0;
    int ind_coluna = 0;
    for(int i = 0; i < 2*(Height/8)*(Width/8) - 1; i += 2) {
        indices[i] = ind_fileira;
        indices[i + 1] = ind_coluna;
        
        ind_coluna += 8;
        if(ind_coluna >= Width) {
            ind_coluna = 0;
            ind_fileira += 8;
        } 
    }


    return indices;
}



/*Height e Width referem-se às dimensões da matrix Y*/
double ****get_blocos8x8 (Imagem_ycbcr *imagem) {
    int qtd_blocos_y = (imagem->Height/8)*(imagem->Width/8);
    int qtd_blocos_c = (imagem->Height_c/8)*(imagem->Width_c/8);
    double ***blocos8x8_y = (double ***) malloc(qtd_blocos_y*sizeof(double **));
    double ***blocos8x8_cb = (double ***) malloc(qtd_blocos_c*sizeof(double **));
    double ***blocos8x8_cr = (double ***) malloc(qtd_blocos_c*sizeof(double **));


    if(blocos8x8_y == NULL || blocos8x8_cb == NULL || blocos8x8_cr == NULL) {
        printf("Erro na alocação do vetor de blocos8x8!\n");
        return NULL;
    }

    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos8x8_y[i] = (double **) aloca_matrix(1, 8, 8);
    }
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos8x8_cb[i] = (double **) aloca_matrix(1, 8, 8);
        blocos8x8_cr[i] = (double **) aloca_matrix(1, 8, 8);
    }

    int *indices_y = get_indices_blocos(imagem->Height, imagem->Width);
    int *indices_c = get_indices_blocos(imagem->Height_c, imagem->Width_c);


    int indice_atual = 0;
    for(int i = 0; i < qtd_blocos_y; i++) {
        for(int j = 0; j < 8; j++)
            for(int k = 0; k < 8; k++) {
                (blocos8x8_y[i])[j][k] = (imagem->Y)[indices_y[indice_atual] + j][indices_y[indice_atual + 1] + k]; 
            }

        indice_atual += 2;
    }

    indice_atual = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        for(int j = 0; j < 8; j++)
            for(int k = 0; k < 8; k++) {
                (blocos8x8_cb[i])[j][k] = (imagem->Cb)[indices_c[indice_atual] + j][indices_c[indice_atual + 1] + k];
                (blocos8x8_cr[i])[j][k] = (imagem->Cr)[indices_c[indice_atual] + j][indices_c[indice_atual + 1] + k]; 
            }

        indice_atual += 2;
    }

    double ****blocos8x8 = (double ****) malloc(3*sizeof(double ***));
    if(blocos8x8 == NULL) {
        printf("Erro na alocação do vetor de blocos8x8!\n");
        return NULL;
    }

    blocos8x8[0] = blocos8x8_y;
    blocos8x8[1] = blocos8x8_cb;
    blocos8x8[2] = blocos8x8_cr;

    if(indices_y != NULL) free(indices_y);
    if(indices_c != NULL) free(indices_c);
    desaloca_imagem_ycbcr(imagem);

    return blocos8x8;
}


Imagem_ycbcr *blocos_to_imagem(double ****blocos, int Height, int Width, int Height_c, int Width_c) {
    Imagem_ycbcr *imagem = aloca_imagem_ycbcr(Height, Width, Height_c, Width_c);
    int qtd_blocos_y = (imagem->Height/8)*(imagem->Width/8);
    int qtd_blocos_c = (imagem->Height_c/8)*(imagem->Width_c/8);

    int *indices_y = get_indices_blocos(imagem->Height, imagem->Width);
    int *indices_c = get_indices_blocos(imagem->Height_c, imagem->Width_c);

    int indice_atual = 0;
    for(int i = 0; i < qtd_blocos_y; i++) {
        for(int j = 0; j < 8; j++)
            for(int k = 0; k < 8; k++) {
                (imagem->Y)[indices_y[indice_atual] + j][indices_y[indice_atual + 1] + k] =  (blocos[0][i])[j][k]; 
            }

        desaloca_matrix((void **) blocos[0][i], 8, 8);
        indice_atual += 2;
    }


    indice_atual = 0;
    for(int i = 0; i < qtd_blocos_c; i++) {
        for(int j = 0; j < 8; j++)
            for(int k = 0; k < 8; k++) {
                (imagem->Cb)[indices_c[indice_atual] + j][indices_c[indice_atual + 1] + k] = (blocos[1][i])[j][k];
                (imagem->Cr)[indices_c[indice_atual] + j][indices_c[indice_atual + 1] + k] = (blocos[2][i])[j][k];  
            }

        desaloca_matrix((void **) blocos[1][i], 8, 8);
        desaloca_matrix((void **) blocos[2][i], 8, 8);
        indice_atual += 2;
    }

    for(int i = 0; i < 3; i++)
        if(blocos[i] != NULL) free(blocos[i]);

    if(blocos != NULL) free(blocos);
    if(indices_y != NULL) free(indices_y);
    if(indices_c != NULL) free(indices_c);
    
    return imagem;
}


