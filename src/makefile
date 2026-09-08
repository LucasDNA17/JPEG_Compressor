CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

OBJS = BMP_header.o BMP_ColorElements.o DCT.o quant.o CodEntropica.o matrix.o jpeg.o Compressao.o main.o

.INTERMEDIATE: $(OBJS)

all: main

main: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

matrix.o: matrix/matrix.c
	$(CC) $(CFLAGS) -c $< -o $@

CodEntropica.o: CodEntropica/CodEntropica.c
	$(CC) $(CFLAGS) -c $< -o $@

quant.o: quant/quant.c
	$(CC) $(CFLAGS) -c $< -o $@

DCT.o: DCT/DCT.c
	$(CC) $(CFLAGS) -c $< -o $@

BMP_ColorElements.o: BMP_ColorElements/BMP_ColorElements.c
	$(CC) $(CFLAGS) -c $< -o $@

BMP_header.o: BMP_header/BMP_header.c
	$(CC) $(CFLAGS) -c $< -o $@

Compressao.o: Compressao/Compressao.c
	$(CC) $(CFLAGS) -c $< -o $@

jpeg.o: jpeg/jpeg.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./main

clean:
	rm -f $(OBJS) main

.PHONY: all clean run