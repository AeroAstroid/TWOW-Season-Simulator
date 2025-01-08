all:
	gcc -static main.c module/*.c -o bin/simulator -O1 -lm -Wno-unused-result

run:
	bin/simulator