CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

# Objetos principais
OBJS = BMP_header.o BMP_ColorElements.o DCT.o quant.o CodEntropica.o matrix.o jpeg.o Compressao.o main.o

all: main

main: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o main

.PHONY: clean

main.o:
	$(CC) -c -g -lm main.c

matrix.o:
	$(CC) -c -g -lm matrix.c

CodEntropica.o:
	$(CC) -c -g CodEntropica.c

quant.o:
	$(CC) -c -g -lm quant.c

DCT.o:
	$(CC) -c -g DCT.c

BMP_ColorElements.o:
	$(CC) -c -g BMP_ColorElements.c

BMP_header.o:
	$(CC) -c -g BMP_header.c

Compressao.o:
	$(CC) -c -g -lm Compressao.c

jpeg.o:
	$(CC) -c -g -lm jpeg.c

run:
	./main