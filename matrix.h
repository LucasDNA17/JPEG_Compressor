#ifndef MATRIX_H
    #define MATRIX_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <math.h>

    void desaloca_matrix(void **matrix, int Height, int Width);
    void **aloca_matrix(int tipo, int Height, int Width);
    void printaMatrix(double **matrix, int Height, int Width);
    //void multiplicacao_matrix(double matrix1[8][8], double matrix2[8][8], double resultado[8][8]);
    void get_bloco_matrix(double **matrix, double bloco[8][8], int bases[2]);
    void set_bloco_matrix(double **matrix, double bloco[8][8], int bases[2]);
    double **multiplicacao_matrix_esq(double **matrix1,  double matrix2[8][8]);
    double **multiplicacao_matrix_dir(double matrix1[8][8], double **matrix2);


#endif