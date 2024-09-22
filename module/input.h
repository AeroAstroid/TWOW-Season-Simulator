#ifndef INPUT
#define INPUT

#include "contestant.h"

typedef struct SimulationInfo {
	int iteration_target;

	Contestant** base_field;
	int full_contestant_count;

	int* threshold_points;
	int game_stages;

	double elim_rate;
	double life_gain_rate;
	int ensure_less_than_half;

	int life_cap;

	int life_decay_timer;
	double ld_elim_rate;
	double ld_life_gain_rate;

	double score_renormalization;

	int* aggregate_results;
	int total_rounds_taken;
} SimulationInfo;

SimulationInfo* create_sim_info(
	int i_t, Contestant** b_f, int c_count, int* t_pts, int g_s, int* a_r, double e_r, int e_lh, 
	double lg_r, int l_c, int ld_t, double ld_e_r, double ld_lg_r, double s_rn);

void read_game_rules(char* f_name, 
	double* elim_rate, int* ensure_less_half, double* life_gain_rate, int* life_cap,
	int* life_decay_timer, double* ld_elim_rate, double* ld_life_gain_rate,
	double* score_renorm, int** threshold_points, int* game_stages);

void read_program_params(char* f_name, int* iteration_target, int* thread_count);

Contestant** read_all_contestants(char* f_name, int* contestant_count);

#endif