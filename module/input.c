#include <stdio.h>
#include <stdlib.h>
#include "input.h"

SimulationInfo* create_sim_info(int i_t, Contestant** b_f, int c_count, int* t_pts, int g_s, int* a_r, double e_r, int e_lh, double lg_r, double s_rn) {
	SimulationInfo* s = malloc(sizeof(SimulationInfo));

	s->iteration_target = i_t;
	s->base_field = b_f;
	s->full_contestant_count = c_count;
	s->threshold_points = t_pts;
	s->game_stages = g_s;
	s->aggregate_results = a_r;

	s->elim_rate = e_r;
	s->ensure_less_than_half = e_lh;
	s->life_gain_rate = lg_r;
	s->score_renormalization = s_rn;

	return s;
}

void read_game_rules(char* f_name, double* elim_rate, int* ensure_less_half, double* life_gain_rate, double* score_renorm, int** threshold_points, int* game_stages) {
	FILE* f = fopen(f_name, "r");

	if (f == NULL) {
		printf("Error opening game rules file!\n");
		exit(-1);
	}

	char buf[25];

	fscanf(f, "%s", buf);
	fscanf(f, "%lf", elim_rate);
	fscanf(f, "%s", buf);
	fscanf(f, "%d", ensure_less_half);
	fscanf(f, "%s", buf);
	fscanf(f, "%lf", life_gain_rate);
	fscanf(f, "%s", buf);
	fscanf(f, "%lf", score_renorm);
	fscanf(f, "%s", buf);

	
	int read_game_stages = 0;
	int last_read_threshold;

	int reading_status = fscanf(f, "%d", &last_read_threshold);
	*threshold_points = calloc(0, sizeof(int));

	while (reading_status > 0) {
		read_game_stages++;
		
		*threshold_points = realloc(*threshold_points, sizeof(int)*read_game_stages);
		(*threshold_points)[read_game_stages-1] = last_read_threshold;

		reading_status = fscanf(f, "%d", &last_read_threshold);
	}

	*game_stages = read_game_stages;

	fclose(f);
}

void read_program_params(char* f_name, int* iteration_target, int* thread_count) {
	FILE* f = fopen(f_name, "r");

	if (f == NULL) {
		printf("Error opening program parameters file!\n");
		exit(-1);
	}

	char buf[25];

	fscanf(f, "%s", buf);
	fscanf(f, "%d", iteration_target);
	fscanf(f, "%s", buf);
	fscanf(f, "%d", thread_count);

	fclose(f);
}

Contestant** read_all_contestants(char* f_name, int* contestant_count) {
	FILE* f = fopen(f_name, "r");

	if (f == NULL) {
		printf("Error opening contestant data file!\n");
		exit(-1);
	}

	int temp_count = 1;

	while ((fscanf(f, "%*[^\n]"), fscanf(f, "%*c")) != -1)
		temp_count++;

	fseek(f, 0, 0);

	Contestant** base_field = malloc(sizeof(Contestant*) * temp_count);

	for (int i = 0; i < temp_count; i++) {
		base_field[i] = read_one_contestant(f, i);
	}

	fclose(f);

	*contestant_count = temp_count;
	return base_field;
}