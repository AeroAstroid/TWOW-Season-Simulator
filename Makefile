all:
	gcc main.c module/*.c -o bin/simulator -Ofast -lm

run:
	bin/simulator