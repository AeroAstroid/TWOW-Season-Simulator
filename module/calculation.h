#ifndef CALCULATION
#define CALCULATION

#include <time.h>

void seed_xoshiro();
double box_muller(double average, double stdev);
int elim_threshold(int contestants, double elim_rate, int ensure_not_half);
void contestant_qsort(int* ids, double* scores, int start, int end);
int qsort_partition(int* ids, double* scores, int start, int pivot_i);
void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td);

#endif