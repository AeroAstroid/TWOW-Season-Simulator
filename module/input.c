#include <stdio.h>
#include <stdlib.h>
#include "input.h"

SimulationInfo* create_sim_info(
	int i_t, Contestant** b_f, int c_count, int* t_pts, int g_s, int* a_r, double e_r, double p_r,
	int l_c, int ld_t, int ld_f, double ld_1_e_r, double ld_1_p_r, double ld_2_e_r, double ld_2_p_r) {

	SimulationInfo* s = malloc(sizeof(SimulationInfo));

	s->iteration_target = i_t;

	s->base_field = b_f;
	s->full_contestant_count = c_count;

	s->threshold_points = t_pts;
	s->game_stages = g_s;
	
	s->elim_rate = e_r;
	s->prize_rate = p_r;

	s->life_cap = l_c;

	s->life_decay_timer = ld_t;
	s->life_decay_floor = ld_f;

	s->ld_1_elim_rate = ld_1_e_r;
	s->ld_1_prize_rate = ld_1_p_r;

	s->ld_2_elim_rate = ld_2_e_r;
	s->ld_2_prize_rate = ld_2_p_r;

	s->aggregate_results = a_r;
	s->total_rounds_taken = 0;

	return s;
}

void read_game_rules(char* f_name, 
	double* elim_rate, double* prize_rate, int* life_cap, int* life_decay_timer,
	int* life_decay_floor, double* ld_1_elim_rate, double* ld_1_prize_rate,
	double* ld_2_elim_rate, double* ld_2_prize_rate, int** threshold_points,
	int* game_stages) {
	
	FILE* f = fopen(f_name, "r");

	if (f == NULL) {
		printf("Error opening game rules file!\n");
		exit(-1);
	}

	char buf[32];

	fscanf(f, "%s", buf);
	fscanf(f, "%lf", elim_rate);
	fscanf(f, "%s", buf);
	fscanf(f, "%lf", prize_rate);

	fscanf(f, "%s", buf);
	fscanf(f, "%d", life_cap);

	fscanf(f, "%s", buf);
	fscanf(f, "%d", life_decay_timer);
	fscanf(f, "%s", buf);
	fscanf(f, "%d", life_decay_floor);

	fscanf(f, "%s", buf);
	fscanf(f, "%lf", ld_1_elim_rate);
	fscanf(f, "%s", buf);
	fscanf(f, "%lf", ld_1_prize_rate);

	fscanf(f, "%s", buf);
	fscanf(f, "%lf", ld_2_elim_rate);
	fscanf(f, "%s", buf);
	fscanf(f, "%lf", ld_2_prize_rate);

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

	char buf[32];

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