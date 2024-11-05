#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define _2e32 0xFFFFFFFF
#define TAU 6.283185

static uint64_t s[4];

void seed_xoshiro() {
	struct timespec seed_time;
	clock_gettime(CLOCK_REALTIME, &seed_time);
	s[0] = seed_time.tv_nsec;
	s[1] = seed_time.tv_nsec;
	s[2] = seed_time.tv_nsec;
	s[3] = seed_time.tv_nsec;
}

uint64_t next(void) {
	const uint64_t result = s[0] + s[3];

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = (s[3] << 45) | (s[3] >> (64 - 45));

	return result;
}

double multiplied_box_muller(double a1, double s1, double a2, double s2) {
	double z;

	do {
		uint64_t r = next();
		double x = ((double)(r >> 32)/_2e32);
		double y = ((double)(r & _2e32)/_2e32);

		double log_term = -2 * log(x);
		double trig_term = sin(TAU * y);

		z = (a1*a2) + (a1*s2 + a2*s1) * sqrt(log_term) * trig_term + (s1*s2) * log_term * trig_term * trig_term;
	} while (z < 0 || z > 1);

	return z;
}

int elim_threshold(int contestants, double elim_rate, int ensure_not_half) {
	int threshold = contestants - (int)round(contestants * elim_rate);

	if (ensure_not_half && threshold <= ((float)contestants / 2)) {
		threshold = (int)floor((float)contestants / 2 + 1);
	}

	if (threshold == contestants) threshold = contestants - 1;

	return threshold;
}

int qsort_partition(int* ids, double* scores, int start, int pivot_i) {
	double pivot_score = scores[ids[pivot_i]];
	int i = (start - 1);
	int temp;

	for (int j = start; j <= pivot_i - 1; j++) {
		if (scores[ids[j]] > pivot_score) {
			i++;

			temp = ids[i];
			ids[i] = ids[j];
			ids[j] = temp;
		}
	}
	
	temp = ids[i + 1];
	ids[i + 1] = ids[pivot_i];
	ids[pivot_i] = temp;

	return i + 1;
}

void contestant_qsort(int* ids, double* scores, int start, int end) {
	if (start < end) {
		int pivot_i = qsort_partition(ids, scores, start, end);

		contestant_qsort(ids, scores, start, pivot_i-1);
		contestant_qsort(ids, scores, pivot_i+1, end);
	}
}

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td) {
	td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
	td->tv_sec  = t2.tv_sec - t1.tv_sec;
	
	if (td->tv_sec > 0 && td->tv_nsec < 0) {
		td->tv_nsec += 1000000000L;
		td->tv_sec--;

	} else if (td->tv_sec < 0 && td->tv_nsec > 0) {
		td->tv_nsec -= 1000000000L;
		td->tv_sec++;
	}
}