#ifndef CONTESTANT
#define CONTESTANT

typedef struct Contestant {
	int id;
	char name[76];

	double average;
	double stdev;

	int start_lives;
} Contestant;

Contestant* read_one_contestant(FILE* f, int id);

#endif