#ifndef CALCULATION
#define CALCULATION

#include <time.h>

void seed_xoshiro();

double multiplied_box_muller(double a1, double s1, double a2, double s2);

int elim_threshold(int contestants, double elim_rate);

void contestant_qsort(int* ids, double* scores, int start, int end);
int qsort_partition(int* ids, double* scores, int start, int pivot_i);

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td);

#endif