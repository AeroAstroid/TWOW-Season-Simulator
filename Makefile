all:
	gcc main.c module/*.c -o simulator -lm

run:
	./simulator