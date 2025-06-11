#ifndef MATRIX_H
    #define MATRIX_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <math.h>

    //Função que desaloca uma matriz.
    //Entrada: ponteiro duplo para o tipo void (é preciso que o ponteiro para a matriz
    //fornecida seja convertido para o tipo void na chamada da função); altura e largura
    //da matriz.
    //Saída: nenhuma.
    void desaloca_matrix(void **matrix, int Height, int Width);
    

    //Função que aloca uma matriz de unsigned chars ou de doubles.
    //Entrada: tipo da matriz alocada (0 - unsigned char; 1 - double); altura e largura da
    //matriz.
    //Saída: ponteiro duplo para void que aponta para a matriz alocada.
    void **aloca_matrix(int tipo, int Height, int Width);


    //Função que multiplica uma matriz A alocada na heap por uma matriz B
    //na memória stack (Multiplicação = A . B).
    //Entrada: matriz 8x8 A armazenada na heap; matriz 8x8 B armazenada na stack.
    //Saída: matrix 8x8 armazenada na heap que é o resultado da multiplicação. 
    double **multiplicacao_matrix_esq(double **matrix1,  double matrix2[8][8]);


    //Função que multiplica uma matriz A alocada na stack por uma matriz B
    //na memória heap (Multiplicação = A . B).
    //Entrada: matriz 8x8 A armazenada na stack; matriz 8x8 B armazenada na heap.
    //Saída: matrix 8x8 armazenada na heap que é o resultado da multiplicação. 
    double **multiplicacao_matrix_dir(double matrix1[8][8], double **matrix2);


#endif