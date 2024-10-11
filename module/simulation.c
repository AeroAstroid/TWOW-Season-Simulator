#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "contestant.h"
#include "calculation.h"
#include "input.h"

#define LOCK_THREADS pthread_mutex_lock(&mutex)
#define UNLOCK_THREADS pthread_mutex_unlock(&mutex)

int iterations_started = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Strength of renormalization for the score model
// 0 -> unmodified gaussian RNG (old style of TWOW sims, horrifically unbalanced)
double SCORE_RENORMALIZATION = 0.28;

void* simulation_thread(void* void_s) {
	// convert the void pointer to the appropriate struct
	SimulationInfo* s = (SimulationInfo*) void_s;

	// run seasons until enough simulations have started to cover the iteration count
	while (iterations_started < s->iteration_target) {

		// lock mutex to increment this variable
		LOCK_THREADS;
			iterations_started++;
		UNLOCK_THREADS;

		// copy the lives data for all contestants
		int current_lives[s->full_contestant_count];

		// [CURRENTLY UNUSED] DRP level array for all contestants
		int drp_level[s->full_contestant_count];

		// final ranks data for all contestants
		int final_ranks[s->full_contestant_count];

		for (int i = 0; i < s->full_contestant_count; i++) {
			// Default starting life count for each contestant is whatever they have in the data
			current_lives[i] = s->base_field[i]->start_lives;

			// Default DRP level for each contestant is 0 (single response)
			drp_level[i] = 0;

			// Default final rank for each contestant is -1 (still alive)
			final_ranks[i] = -1;
		}
		
		// last simulated score for every contestant
		double round_scores[s->full_contestant_count];

		// list of all contestant IDs that are currently alive
		int alive_ids[s->full_contestant_count];

		// Rounds until life decay starts (if -1, no life decay)
		int life_decay_timer = s->life_decay_timer;

		int current_alive_count;
		double current_elim_rate;
		double current_prize_rate;

		int simulated_round_count = 0;
		
		do {
			// if life decay timer is positive, remove a round until it starts
			if (life_decay_timer > 0) life_decay_timer--;

			// determine elim and prize rates for the round
			current_elim_rate = (life_decay_timer != 0) ? s->elim_rate : s->ld_elim_rate;
			current_prize_rate  = (life_decay_timer != 0) ? s->life_gain_rate : s->ld_life_gain_rate;

			current_alive_count = 0;
			for (int i = 0; i < s->full_contestant_count; i++) {
				if (current_lives[i] > 0) {
					alive_ids[current_alive_count] = s->base_field[i]->id;
					current_alive_count++;
				}
			}

			for (int cont = 0; cont < current_alive_count; cont++) {
				int c_id = alive_ids[cont];

				round_scores[c_id] = -99999;

				// simulate performance by rolling gaussian rng for each response the player has
				// pick the highest as per standard DRP/TRP rules
				for (int resp = 0; resp <= drp_level[c_id]; resp++) {

					double response_score = multiplied_box_muller(
						s->base_field[c_id]->average, s->base_field[c_id]->stdev,
						0.5, SCORE_RENORMALIZATION);
					
					if (response_score > round_scores[c_id]) round_scores[c_id] = response_score;
				}
			}

			// sort alive contestants based on their simulated performance
			contestant_qsort(alive_ids, round_scores, 0, current_alive_count-1);

			// wipe slate clean for next round's DRP data
			for (int i = 0; i < s->full_contestant_count; i++) {
				drp_level[i] = 0;
			}

			// remove a life from the ones in the elim zone
			int round_elim_threshold = elim_threshold(current_alive_count, current_elim_rate, s->ensure_less_than_half);
			for (int r = current_alive_count - 1; r >= round_elim_threshold; r--) {
				int c_id = alive_ids[r];
				current_lives[c_id]--;

				if (current_lives[c_id] == 0) {
					final_ranks[c_id] = current_alive_count;
					current_alive_count--;
				}
			}

			// add a life from the ones in the life gain zone
			int round_life_gain_threshold = round(current_alive_count * current_prize_rate);
			for (int r = 0; r < round_life_gain_threshold; r++) {
				int c_id = alive_ids[r];

				// add life if they're not already at the cap AND if life decay isn't active
				if ((s->life_cap <= 0 || current_lives[c_id] < s->life_cap) && life_decay_timer != 0)
					current_lives[c_id]++;
			}

			// post-results life decay if the life decay phase is active
			if (life_decay_timer == 0) {
				for (int c = 0; c < s->full_contestant_count; c++) {
					if (current_lives[c] > s->life_decay_floor) {
						current_lives[c]--;
					}
				}
			}

			simulated_round_count++;

		} while (current_alive_count > 1);

		final_ranks[alive_ids[0]] = 1;

		for (int i = 0; i < s->full_contestant_count; i++) {
			int rank = final_ranks[i];

			if (rank == -1) continue;

			for (int st = 0; st < s->game_stages; st++) {
				if (rank <= s->threshold_points[st]) {
					LOCK_THREADS;
						s->aggregate_results[i*s->game_stages+st]++;
					UNLOCK_THREADS;
				}
			}
		}

		// increment global round counter
		LOCK_THREADS;
			s->total_rounds_taken += simulated_round_count;
		UNLOCK_THREADS;
	}
}