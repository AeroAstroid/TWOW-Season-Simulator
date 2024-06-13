all:
	gcc main.c module/*.c -o bin/simulator -Ofast -lm -Wno-unused-result

run:
	bin/simulator