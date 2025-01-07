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
	double prize_rate;

	int life_cap;

	int life_decay_timer;
	int life_decay_floor;

	double ld_1_elim_rate;
	double ld_1_prize_rate;

	double ld_2_elim_rate;
	double ld_2_prize_rate;

	int udrp_threshold;
	int utrp_threshold;

	int* aggregate_results;
	int total_rounds_taken;
} SimulationInfo;

SimulationInfo* create_sim_info(
	int i_t, Contestant** b_f, int c_count, int* t_pts, int g_s, int* a_r, double e_r, double p_r,
	int l_c, int ld_t, int ld_f, double ld_1_e_r, double ld_1_p_r, double ld_2_e_r, double ld_2_p_r,
	int udrp, int utrp);

void read_game_rules(char* f_name, 
	double* elim_rate, double* prize_rate, int* life_cap, int* life_decay_timer,
	int* life_decay_floor, double* ld_1_elim_rate, double* ld_1_prize_rate,
	double* ld_2_elim_rate, double* ld_2_prize_rate, int* udrp_threshold, int* utrp_threshold,
	int** threshold_points, int* game_stages);

void read_program_params(char* f_name, int* iteration_target, int* thread_count);

Contestant** read_all_contestants(char* f_name, int* contestant_count);

#endif