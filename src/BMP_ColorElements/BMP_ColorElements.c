#include "BMP_ColorElements.h"


//Macros de máximo e mínimo
#define max(a,b) ((a > b) ? (a) : (b))
#define min(a,b) ((a < b) ? (a) : (b))


/* ----- Funções de alocação/desalocação de memória de imagens RGB e YCbCr ----- */


//Função que desaloca uma struct de imagem RGB.
//Entrada: ponteiro para a struct de imagem RGB a ser desalocada.
//Saída: nenhuma.
void desaloca_imagem_rgb(Imagem_rgb *imagem) {
    if(imagem == NULL)
        return;

    //Desaloca a matrix do componente R.
    desaloca_matrix((void **)imagem->R, imagem->Height, imagem->Width);
    //Desaloca a matrix do componente G.
    desaloca_matrix((void **)imagem->G, imagem->Height, imagem->Width);
    //Desaloca a matrix do componente B.
    desaloca_matrix((void **)imagem->B, imagem->Height, imagem->Width);
    //Desaloca a imagem.
    free(imagem);
    imagem = NULL;
}


//Função que desaloca uma struct de imagem YCbCr.
//Entrada: ponteiro para a struct de imagem YCbCr a ser desalocada.
//Saída: nenhuma.
void desaloca_imagem_ycbcr(Imagem_ycbcr *imagem) {
    if(imagem == NULL)
        return;

    //Desaloca a matrix do componente Y.
    desaloca_matrix((void **)imagem->Y, imagem->Height, imagem->Width);
    //Desaloca a matrix do componente Cb.
    desaloca_matrix((void **)imagem->Cb, imagem->Height_c, imagem->Width_c);
    //Desaloca a matrix do componente Cr.
    desaloca_matrix((void **)imagem->Cr, imagem->Height_c, imagem->Width_c);
    //Desaloca a imagem.
    free(imagem);
    imagem = NULL;
}


//Função que aloca uma struct de imagem RGB, bem como as matrizes de seus componentes.
//Entrada: altura e largura da imagem.
//Saída: ponteiro para a struct de imagem RGB criada.
Imagem_rgb *aloca_imagem_rgb(int Height, int Width) {
    //Aloca a imagem.
    Imagem_rgb *imagem = (Imagem_rgb *) malloc(sizeof(Imagem_rgb));
    if(imagem == NULL) {
        printf("Erro de alocação de imagem!\n");
        return NULL;
    }

    //Registra os parâmetros da imagem na struct
    imagem->Height = Height;
    imagem->Width = Width;
    //Aloca a matrix da componente R.
    imagem->R = (unsigned char **) aloca_matrix(0, imagem->Height, imagem->Width);
    //Aloca a matrix da componente G.
    imagem->G = (unsigned char **) aloca_matrix(0, imagem->Height, imagem->Width);
    //Aloca a matrix da componente B.
    imagem->B = (unsigned char **) aloca_matrix(0, imagem->Height, imagem->Width);

    return imagem; 
}


//Função que aloca uma struct de imagem YCbCr, bem como as matrizes de seus componentes.
//Entrada: altura e largura das matrizes da componente de luminância; altura e largura das
//componentes de crominância.
//Saída: ponteiro para a struct de imagem YCbCr criada.
Imagem_ycbcr *aloca_imagem_ycbcr(int Height, int Width, int Height_c, int Width_c) {
    //Aloca a imagem.
    Imagem_ycbcr *imagem = (Imagem_ycbcr *) malloc(sizeof(Imagem_ycbcr));
    if(imagem == NULL) {
        printf("Erro de alocação de imagem!\n");
        return NULL;
    }

    //Registra os parâmetros da imagem na struct
    imagem->Height = Height;
    imagem->Width = Width;
    imagem->Height_c = Height_c;
    imagem->Width_c = Width_c;
    //Aloca a matrix da componente Y.
    imagem->Y = (double **) aloca_matrix(1, imagem->Height, imagem->Width);
    //Aloca a matrix da componente Cb.
    imagem->Cb = (double **) aloca_matrix(1,imagem->Height_c, imagem->Width_c);
    //Aloca a matrix da componente Cr.
    imagem->Cr = (double **) aloca_matrix(1,imagem->Height_c, imagem->Width_c);
    return imagem;
}


/* ----- Funções de leitura/escrita de imagem ----- */


//Função que lê uma imagem BMP no formato RGB e retorna uma struct desse formato. A leitura é
//feita pixel a pixel seguindo a ordem de componentes: Blue, Red, Green.
//Entrada: ponteiro para o arquivo BMP; altura e comprimento da imagem
//Saída: ponteiro para struct Imagem_rgb que armazena a imagem. 
Imagem_rgb *getImage(FILE *F, int Height, int Width) {
    Imagem_rgb *imagem = aloca_imagem_rgb(Height, Width);

    //Pula o cabeçalho do arquivo.
    fseek(F, BMP_HEADER_SIZE, SEEK_SET);
    //Lê pixel a pixel a imagem do arquivo.
    for(int i = 0; i < Height; i++)
        for(int j = 0; j < Width; j++) {
            //Lê a componente B.
            fread(&((imagem->B)[i][j]), sizeof(unsigned char), 1, F);
            //Lê a componente G.
            fread(&((imagem->G)[i][j]), sizeof(unsigned char), 1, F);
            //Lê a componente R.
            fread(&((imagem->R)[i][j]), sizeof(unsigned char), 1, F);
        }

    return imagem;
}

//Função que escreve em um arquivo BMP a imagem no formato RGB. A escrita é feita pixel
//a pixel segundo a ordem de componentes: Blue, Red, Green.
//Entrada: ponteiro para o arquivo BMP; ponteiro para a struct Imagem_rgb que contém a imagem.
//Saída: nenhuma.
void storeImage(FILE *F, Imagem_rgb *imagem) {
    //Armazena pixel a pixel a imagem no arquivo.
    for(int i = 0; i < imagem->Height; i++)
        for(int j = 0; j < imagem->Width; j++) {
            //Armazena a componente B.
            fwrite(&((imagem->B)[i][j]), sizeof(unsigned char), 1, F);
            //Armazena a componente G.
            fwrite(&((imagem->G)[i][j]), sizeof(unsigned char), 1, F);
            //Armazena a componente R.
            fwrite(&((imagem->R)[i][j]), sizeof(unsigned char), 1, F);
        }

    //Desaloca a struct de imagem RGB que continha a imagem.
    desaloca_imagem_rgb(imagem);
    return;
}


/* ----- Funções de conversão RGB/YCbCr ----- */


//Função que "converte" uma struct de imagem RGB para uma no formato YCbCr.
//As fórmulas de conversão utilizadas foram retiradas da documentação da biblioteca OpenCV:
//  - Y = 0.299R + 0.587G + 0.114B
//  - Cb = 0.564(B − Y ) + 128
//  - Cr = 0.713(R − Y ) + 128
//Entrada: ponteiro para a struct Imagem_rgb.
//Saída: ponteiro para a struct Imagem_Ycbcr que contém os pixels convertidos.
Imagem_ycbcr *RGBtoYCbCr(Imagem_rgb *imagem) {
    //Aloca a struct de imagem YCbCr.
    Imagem_ycbcr *converted_image = aloca_imagem_ycbcr(imagem->Height, imagem->Width, imagem->Height, imagem->Width);
   
    //Converte a imagem RGB pixel a pixel para uma imagem no formato YCbCr.
    for(int i = 0; i < converted_image->Height; i++) {
        for(int j = 0; j < converted_image->Width; j++) {
            //Faz o "casting" das componentes unsigned char da imagem RGB para o tipo double.
            double R = (double) (imagem->R)[i][j];
            double G = (double) (imagem->G)[i][j];
            double B = (double) (imagem->B)[i][j];
            
            //Converte as componentes de acordo com as fórmulas fornecidas. Os macros de min e max garantem
            //que os valores convertidos estejam no intervalo [0,255].
            (converted_image->Y)[i][j] = min(max(0.299*R + 0.587*G + 0.114*B, 0), 255);
            (converted_image->Cb)[i][j] = min(max(0.564*(B - converted_image->Y[i][j]) + 128, 0), 255); 
            (converted_image->Cr)[i][j] = min(max(0.713*(R - converted_image->Y[i][j]) + 128, 0), 255);

        }
    }

    //Desaloca a imagem RGB usada.
    desaloca_imagem_rgb(imagem);
    return converted_image;
}


//Função que "converte" uma struct de imagem YCbCr para uma no formato RGB.
//As fórmulas de conversão utilizadas foram retiradas da documentação da biblioteca OpenCV:
//  - R = Y + 1.403.(Cr - 128)
//  - G = Y − 0.344.(Cb - 128) − 0.714.(Cr - 128)
//  - B = Y + 1.773.(Cb - 128) 
//Entrada: ponteiro para a struct Imagem_ycbcr.
//Saída: ponteiro para a struct Imagem_rgb que contém os pixels convertidos.
Imagem_rgb *YCbCrtoRGB(Imagem_ycbcr *imagem) {
    //Aloca a struct de imagem RGB.
    Imagem_rgb *converted_image = aloca_imagem_rgb(imagem->Height, imagem->Width);
    
    //Converte a imagem YCbCr pixel a pixel para uma imagem no formato RGB.
    for(int i = 0; i < imagem->Height; i++)
        for(int j = 0; j < imagem->Width; j++) {
            
            //Simplificação da escrita, representando Cb - 128 e Cr - 128
            double Cb_shifted = (imagem->Cb)[i][j] - 128;
            double Cr_shifted = (imagem->Cr)[i][j] - 128;
            
            //Conversão de acordo com as fórmulas apresentadas.
            double R = (imagem->Y)[i][j] + 1.403 * Cr_shifted;
            double G = (imagem->Y)[i][j] - 0.344 * Cb_shifted - 0.714 * Cr_shifted;
            double B = (imagem->Y)[i][j] + 1.773 * Cb_shifted;
            
            //Arredondamento dos doubles R,G,B para o tipo unsigned char.
            //Os macros min e max garantem que os valores estejam no intervalo [0,255].
            (converted_image->R)[i][j] = min(max(round(R), 0), 255);
            (converted_image->G)[i][j] = min(max(round(G), 0), 255);
            (converted_image->B)[i][j] = min(max(round(B), 0), 255);
        }

    //Desaloca a imagem YCbCr utilizada.
    desaloca_imagem_ycbcr(imagem);
    return converted_image;
}


/* ----- Funções de downsampling/upsampling ----- */


//Função que realiza o downsampling 4:2:0 em uma imagem no formato YCbCr.
//Entrada: ponteiro para a struct Imagem_ycbcr que contém a imagem.
//Saída: nenhuma.
void downsampling(Imagem_ycbcr *imagem) {
    if(imagem == NULL)
        return;

    //Inicializa as variáveis das dimensões dos canais de crominância.
    int tam_vertical = imagem->Height;
    int tam_horizontal = imagem->Width;

    //Redução das dimensões do canal de crominância pela metade.
    tam_vertical /= 2;
    tam_horizontal  /= 2;

    //Correção das dimensões para que elas sejam múltiplas de 8, o que é
    //necessário para a divisão da imagem em blocos 8x8.
    if(tam_vertical % 8 != 0)
        tam_vertical += 8 - (tam_vertical % 8);
    if(tam_horizontal % 8 != 0)
        tam_horizontal += 8 - (tam_horizontal % 8);
    
    //Atualização das dimensões dos canais de crominância na struct da imagem.
    imagem->Height_c = tam_vertical;
    imagem->Width_c = tam_horizontal;

    //Criação das matrizes dos canais de crominância com as novas dimensões. 
    double **matrix_Cb = (double **) aloca_matrix(1, tam_vertical, tam_horizontal);
    double **matrix_Cr = (double **) aloca_matrix(1, tam_vertical, tam_horizontal);
    
    //Escrita da médias dos pixels das matrizes originais para as matrizes criadas.
    //A cada 4 pixels de crominância, é salva apenas a médias deles de acordo com
    //o subsampling 4:2:0.
    int contador_fileira = 0;
    int contador_coluna = 0;
    for(int i = 0; i < imagem->Height; i+= 2) {
        contador_coluna = 0;
        for(int j = 0; j < imagem->Width; j+= 2) {
            //Quatro pixels do canal Cb.
            double a = (imagem->Cb)[i][j];
            double b = (imagem->Cb)[i][j + 1];
            double c = (imagem->Cb)[i + 1][j];
            double d = (imagem->Cb)[i + 1][j + 1];

            //Quatro pixels do canal Cr.
            double a0 = (imagem->Cr)[i][j];
            double b0 = (imagem->Cr)[i][j + 1];
            double c0 = (imagem->Cr)[i + 1][j];
            double d0 = (imagem->Cr)[i + 1][j + 1];

            //Média dos pixels do canal Cb.
            double mediab = (a + b + c + d)/4;
            mediab = min(max(mediab, 0), 255); //Garantia de que o valor está no intervalo [0,255]
            //Média dos pixels do canal Cr.
            double mediar = (a0 + b0 + c0 + d0)/4;
            mediar = min(max(mediar, 0), 255); //Garantia de que o valor está no intervalo [0,255]
 
            //Escreve as médias dos pixels nas novas matrizes.
            matrix_Cb[contador_fileira][contador_coluna] = mediab;
            matrix_Cr[contador_fileira][contador_coluna] = mediar;

            contador_coluna++;

        }
        contador_fileira++;
    }


    //O espaço que sobra (advindo da correção das dimensões para múltiplos de 8) é preenchido com colunas ou
    //fleiras adjacentes.

    //Colunas ainda vazias em fileiras que não estão vazias tornam-se cópias das colunas adjacentes.
    for(int i = 0; i < imagem->Height/2; i++)
        for(int j = imagem->Width/2; j < tam_horizontal; j++) {
            matrix_Cb[i][j] = matrix_Cb[i][j - 1];
            matrix_Cr[i][j] = matrix_Cr[i][j - 1];
        }

    //Fileiras ainda vazias tornam-se cópias de fileiras adjacentes.
    for(int i = imagem->Height/2; i < tam_vertical; i++)
        for(int j = 0; j < tam_horizontal; j++) {
            matrix_Cb[i][j] = matrix_Cb[i - 1][j];
            matrix_Cr[i][j] = matrix_Cr[i - 1][j];
        }

    
    //Desaloca as matrizes antigas dos canais de crominância.
    desaloca_matrix((void **)imagem->Cb, imagem->Height, imagem->Width);
    desaloca_matrix((void **)imagem->Cr, imagem->Height, imagem->Width);

    //Atualiza as matrizes da struct que armazena a imagem.
    imagem->Cb = matrix_Cb;
    imagem->Cr = matrix_Cr;
}




//Função que realiza o upsampling em uma imagem no formato YCbCr.
//Entrada: ponteiro para a struct Imagem_ycbcr que contém a imagem.
//Saída: nenhuma.
void upsampling(Imagem_ycbcr *imagem) {
    if(imagem == NULL)
        return;

    //Aloca matrizes para os canais de crominância com as dimensões originais da imagem.
    double **matrix_Cb = (double **) aloca_matrix(1, imagem->Height, imagem->Width);
    double **matrix_Cr = (double **) aloca_matrix(1, imagem->Height, imagem->Width);

    int fileira = 0, coluna = 0;
    
    //Escrita dos pixels nas matrizes criadas. A cada pixel nas matrizes reduzidas, o valor dele
    //é copiado para quatro pixels nas matrizes criadas.
    for(int i = 0; i < imagem->Height; i += 2) {
        coluna = 0;
        for(int j = 0; j < imagem->Width; j += 2) {
            //Verifica se os limites das matrizes reduzidas não foram ultrapassados.
            if(i + 1 < imagem->Height && j + 1 < imagem->Width && 
               fileira < imagem->Height_c && coluna < imagem->Width_c) {
                
                double valor_cr = (imagem->Cr)[fileira][coluna];
                double valor_cb = (imagem->Cb)[fileira][coluna];
                
                //Escrita do valor Cr da matriz reduzidas nos quatro pixels da matriz criada.
                matrix_Cr[i][j] = valor_cr;
                matrix_Cr[i + 1][j] = valor_cr;
                matrix_Cr[i][j + 1] = valor_cr;
                matrix_Cr[i + 1][j + 1] = valor_cr;
                
                //Escrita do valor Cr da matriz reduzidas nos quatro pixels da matriz criada.
                matrix_Cb[i][j] = valor_cb;
                matrix_Cb[i + 1][j] = valor_cb;
                matrix_Cb[i][j + 1] = valor_cb;
                matrix_Cb[i + 1][j + 1] = valor_cb;
            }
            coluna++;
        }
        fileira++;
    }

    //Desaloca as matrizes reduzidas.
    desaloca_matrix((void **)imagem->Cb, (imagem->Height_c), (imagem->Width_c));
    desaloca_matrix((void **)imagem->Cr, (imagem->Height_c), (imagem->Width_c));

    //Atualiza as matrizes contidas na struct.
    imagem->Height_c = imagem->Height;
    imagem->Width_c = imagem->Width;
    imagem->Cb = matrix_Cb;
    imagem->Cr = matrix_Cr;  
}


/* ----- Funções de levelshift ----- */


//Função que realiza o downshift em uma imagem BMP, isto é, subtrai 128 do canal Y de cada pixel.
//Entrada: ponteiro para struct da imagem no formato YCbCr.
//Saída:nenhuma.
void downLevelShift(Imagem_ycbcr *imagem) {
    for(int i = 0; i < imagem->Height; i++) {
        for(int j = 0; j < imagem->Width; j++)
            (imagem->Y)[i][j] -= 128;
    }
}


//Função que realiza o upshift em uma imagem BMP, isto é, adiciona 128 no canal Y de cada pixel.
//Entrada: ponteiro para struct da imagem no formato YCbCr.
//Saída:nenhuma.
void upLevelShift(Imagem_ycbcr *imagem) {
    for(int i = 0; i < imagem->Height; i++) {
        for(int j = 0; j < imagem->Width; j++)
            (imagem->Y)[i][j] += 128;
    }
}


/* ----- Funções de divisão da imagem em blocos 8x8 ----- */


//Função que divide uma imagem BMP no formato YCbCr em blocos 8x8 de pixels.
//Entrada: ponteiro para struct de imagem BMP no formato YCbCr.
//Saída: ponteiro quádruplo para double em que a indexação mais exterior representa o canal de informação
//da imagem (0 - Y; 1 - Cb; 2 - Cr); a segunda mais exterior representa o bloco 8x8 (1º, 2º, etc.); e as últimas
//indexações representam o bloco 8x8 em si, no formato de matriz.
double ****get_blocos8x8 (Imagem_ycbcr *imagem) {
    //Calcula a quantidade de blocos nos canais de luminância e crominância.
    int qtd_blocos_y = (imagem->Height/8)*(imagem->Width/8);
    int qtd_blocos_c = (imagem->Height_c/8)*(imagem->Width_c/8);
    //Alocação dos vetores de blocos 8x8 em cada canal de informação.
    double ***blocos8x8_y = (double ***) malloc(qtd_blocos_y*sizeof(double **));
    double ***blocos8x8_cb = (double ***) malloc(qtd_blocos_c*sizeof(double **));
    double ***blocos8x8_cr = (double ***) malloc(qtd_blocos_c*sizeof(double **));

    //Verifica se os vetores foram alocados corretamente.
    if(blocos8x8_y == NULL || blocos8x8_cb == NULL || blocos8x8_cr == NULL) {
        printf("Erro na alocação do vetor de blocos8x8!\n");
        return NULL;
    }

    //Alocação dos blocos 8x8 nos canais de crominância e luminância.
    for(int i = 0; i < qtd_blocos_y; i++) {
        blocos8x8_y[i] = (double **) aloca_matrix(1, 8, 8);
    }
    for(int i = 0; i < qtd_blocos_c; i++) {
        blocos8x8_cb[i] = (double **) aloca_matrix(1, 8, 8);
        blocos8x8_cr[i] = (double **) aloca_matrix(1, 8, 8);
    }

    
    //Extração dos blocos 8x8 do canal Y.
    int bloco_idx = 0;
    for(int linha_bloco = 0; linha_bloco < imagem->Height/8; linha_bloco++) {
        for(int coluna_bloco = 0; coluna_bloco < imagem->Width/8; coluna_bloco++) {
            //Índices do primeiro elemento do bloco 8x8. 
            int linha_base = linha_bloco * 8;
            int coluna_base = coluna_bloco * 8;
            
            //Extração dos elementos do bloco 8x8.
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    (blocos8x8_y[bloco_idx])[i][j] = (imagem->Y)[linha_base + i][coluna_base + j];
                }
            }
            bloco_idx++;
        }
    }

    //Extração dos blocos 8x8 dos canais Cb e Cr.
    bloco_idx = 0;
    for(int linha_bloco = 0; linha_bloco < imagem->Height_c/8; linha_bloco++) {
        for(int coluna_bloco = 0; coluna_bloco < imagem->Width_c/8; coluna_bloco++) {
            //Índices do primeiro elemento do bloco 8x8. 
            int linha_base = linha_bloco * 8;
            int coluna_base = coluna_bloco * 8;
            
            //Extração dos elementos do bloco 8x8.
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    (blocos8x8_cb[bloco_idx])[i][j] = (imagem->Cb)[linha_base + i][coluna_base + j];
                    (blocos8x8_cr[bloco_idx])[i][j] = (imagem->Cr)[linha_base + i][coluna_base + j];
                }
            }
            bloco_idx++;
        }
    }

    //Alocação do vetor que armazena os canais de informação.
    double ****blocos8x8 = (double ****) malloc(3*sizeof(double ***));
    if(blocos8x8 == NULL) {
        printf("Erro na alocação do vetor de blocos8x8!\n");
        return NULL;
    }

    //Escrita dos canais de informação no vetor criado.
    blocos8x8[0] = blocos8x8_y;
    blocos8x8[1] = blocos8x8_cb;
    blocos8x8[2] = blocos8x8_cr;

    //Desaloca a struct de imagem YCbCr utilizada.
    desaloca_imagem_ycbcr(imagem);

    return blocos8x8;
}

//Função que cria uma struct de imagem BMP a partir de blocos 8x8.
//Entrada: ponteiro quádruplo para double que armazena os blocos 8x8 (conforme especificado na sáida da função get_blocos8x8);
//altura e largura da imagem no canal Y ("Height" e "Width"); altura e largura da imagem nos canais Cb e Cr ("Height_c" e "Width_c").
//Saída: ponteiro para struct imagem BMP no formato YCbCr que armazena a imagem reconstruída.
Imagem_ycbcr *blocos_to_imagem(double ****blocos, int Height, int Width, int Height_c, int Width_c) {
    //Aloca a struct de imagem YCbCr.
    Imagem_ycbcr *imagem = aloca_imagem_ycbcr(Height, Width, Height_c, Width_c);

    //Cálculo da quantidade de blocos nos canais de luminância e crominância.
    int qtd_blocos_y = (imagem->Height/8)*(imagem->Width/8);
    int qtd_blocos_c = (imagem->Height_c/8)*(imagem->Width_c/8);

    //Reconstrói o canal Y a partir dos blocos 8x8.
    int bloco_idx = 0;
    for(int linha_bloco = 0; linha_bloco < Height/8; linha_bloco++) {
        for(int coluna_bloco = 0; coluna_bloco < Width/8; coluna_bloco++) {
            //Índices do primeiro elemento do bloco 8x8. S
            int linha_base = linha_bloco * 8;
            int coluna_base = coluna_bloco * 8;
            
            //Reconstrói os elementos do bloco 8x8.
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    (imagem->Y)[linha_base + i][coluna_base + j] = (blocos[0][bloco_idx])[i][j];
                }
            }
            bloco_idx++;
        }
    }

    //Reconstrói os canais Cb e Cr a partir dos blocos 8x8. 
    bloco_idx = 0;
    for(int linha_bloco = 0; linha_bloco < Height_c/8; linha_bloco++) {
        for(int coluna_bloco = 0; coluna_bloco < Width_c/8; coluna_bloco++) {
            //Índices do primeiro elemento do bloco 8x8. 
            int linha_base = linha_bloco * 8;
            int coluna_base = coluna_bloco * 8;
            
            //Reconstrói os elementos do bloco 8x8.
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < 8; j++) {
                    (imagem->Cb)[linha_base + i][coluna_base + j] = (blocos[1][bloco_idx])[i][j];
                    (imagem->Cr)[linha_base + i][coluna_base + j] = (blocos[2][bloco_idx])[i][j];
                }
            }
            bloco_idx++;
        }
    }

    //Desaloca os blocos 8x8 dos canais de informação.
    for(int i = 0; i < qtd_blocos_y; i++) {
        desaloca_matrix((void **) blocos[0][i], 8, 8);
    }
    for(int i = 0; i < qtd_blocos_c; i++) {
        desaloca_matrix((void **) blocos[1][i], 8, 8);
        desaloca_matrix((void **) blocos[2][i], 8, 8);
    }

    //Desaloca os vetores de bloco de cada canal de informação.
    for(int i = 0; i < 3; i++)
        if(blocos[i] != NULL) free(blocos[i]);

    //Desaloca o vetor que representa os canais de informação.
    if(blocos != NULL) free(blocos);
    
    return imagem;
}