all:
	gcc main.c module/*.c -o bin/simulator -lm

run:
	bin/simulator