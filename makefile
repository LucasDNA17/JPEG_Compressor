all: main.o BMP_header.o BMP_ColorElements.o DCT.o quant.o CodEntropica.o matrix.o
	gcc main.o BMP_header.o BMP_ColorElements.o DCT.o quant.o CodEntropica.o matrix.o -o main -g -lm -std=c99 -Werror


main.o:
	gcc -c -g -lm main.c

matrix.o:
	gcc -c -g -lm matrix.c

CodEntropica.o:
	gcc -c -g CodEntropica.c

quant.o:
	gcc -c -g -lm quant.c

DCT.o:
	gcc -c -g DCT.c

BMP_ColorElements.o:
	gcc -c -g BMP_ColorElements.c

BMP_header.o:
	gcc -c -g BMP_header.c

clean:
	rm *.o
	rm main

run:
	./main