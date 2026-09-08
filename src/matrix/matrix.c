#include "matrix.h"


//Função que desaloca uma matriz.
//Entrada: ponteiro duplo para o tipo void (é preciso que o ponteiro para a matriz
//fornecida seja convertido para o tipo void na chamada da função); altura e largura
//da matriz.
//Saída: nenhuma.
void desaloca_matrix(void **matrix, int Height, int Width) {
    if(matrix == NULL)
        return;

    //Desaloca cada vetor da matriz.
    for(int i = 0; i < Height;i++)
        for(int j = 0; j < Width; j++) {
            free(matrix[i]);
            matrix[i] = NULL;
        }

    //Desaloca a matriz.
    free(matrix);
    matrix = NULL;
}

//Função que aloca uma matriz de unsigned chars ou de doubles.
//Entrada: tipo da matriz alocada (0 - unsigned char; 1 - double); altura e largura da
//matriz.
//Saída: ponteiro duplo para void que aponta para a matriz alocada.
void **aloca_matrix(int tipo, int Height, int Width) {
    //Aloca a matriz.
    void **matrix = (void **) malloc(Height*sizeof(void *));
    if(matrix == NULL) {
        printf("Erro na alocação de matrix!\n");
        return NULL;
    }
    
    //Aloca os vetores da matriz, a depender do tipo escolhido.
    for(int i = 0; i < Height; i++) {
        if(tipo == 0) {
            matrix[i] = (unsigned char *) malloc(Width*sizeof(unsigned char));
        }
        else if(tipo == 1)
            matrix[i] = (double *) malloc(Width*sizeof(double));
    }

    return matrix;
}


//Função que multiplica uma matriz A alocada na heap por uma matriz B
//na memória stack (Multiplicação = A . B).
//Entrada: matriz 8x8 A armazenada na heap; matriz 8x8 B armazenada na stack.
//Saída: matrix 8x8 armazenada na heap que é o resultado da multiplicação. 
double **multiplicacao_matrix_esq(double **matrix1,  double matrix2[8][8]) {
    //Aloca a matriz de resultado.
    double **resultado = (double **) aloca_matrix(1, 8, 8);
    //Realiza a multiplicação.
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            resultado[i][j] = 0;
            for(int k = 0; k < 8; k++)
                resultado[i][j] += matrix1[i][k]*matrix2[k][j];
        }
    }

    return resultado;
}

//Função que multiplica uma matriz A alocada na stack por uma matriz B
//na memória heap (Multiplicação = A . B).
//Entrada: matriz 8x8 A armazenada na stack; matriz 8x8 B armazenada na heap.
//Saída: matrix 8x8 armazenada na heap que é o resultado da multiplicação. 
double **multiplicacao_matrix_dir(double matrix1[8][8], double **matrix2) {
    //Aloca a matriz de resultado.
    double **resultado = (double **) aloca_matrix(1, 8, 8);
    //Realiza a multiplicação.
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            resultado[i][j] = 0;
            for(int k = 0; k < 8; k++)
                resultado[i][j] += matrix1[i][k]*matrix2[k][j];
        }
    }
    return resultado;
}