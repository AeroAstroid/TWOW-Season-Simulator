#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contestant.h"

Contestant* read_one_contestant(FILE* f, int id) {
    Contestant* new_contestant = malloc(sizeof(Contestant));
    new_contestant->id = id;

    char parsed_name[76];
    int ptr = 0;

    char current_c = fgetc(f);
    do {
        if (ptr <= 75) {
            parsed_name[ptr] = current_c;
            ptr++;
        }

        current_c = fgetc(f);
    } while (current_c != '\t');

    parsed_name[ptr] = 0;

    if (parsed_name[0] == '\n') {
        memmove(parsed_name, parsed_name+1, strlen(parsed_name));
    }

    strcpy(new_contestant->name, parsed_name);
    fscanf(f, "%lf", &new_contestant->average);
    fscanf(f, "%lf", &new_contestant->stdev);

    current_c = fgetc(f);

    if (current_c == '\t') {
        fscanf(f, "%d", &new_contestant->start_lives);
        fgetc(f); // expend the newline
    } else {
        new_contestant->start_lives = 1;
    }

    return new_contestant;
}