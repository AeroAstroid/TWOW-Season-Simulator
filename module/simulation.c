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

		// DRP level array for all contestants
		int drp_level[s->full_contestant_count];

		// final ranks data for all contestants
		int final_ranks[s->full_contestant_count];

		for (int i = 0; i < s->full_contestant_count; i++) {
			// Default starting life count for each contestant is whatever they have in the data
			current_lives[i] = s->base_field[i]->start_lives;

			// Default DRP level for each contestant is whatever they have in the data
			drp_level[i] = s->base_field[i]->start_responses;

			// Default final rank for each contestant is -1 (still alive)
			final_ranks[i] = -1;
		}
		
		// last simulated score for every contestant
		double round_scores[s->full_contestant_count];

		// list of all contestant IDs that are currently alive
		int alive_ids[s->full_contestant_count];

		int current_alive_count;

		int simulated_round_count = 0;
		
		do {
			// re-index the list of alive contestants
			current_alive_count = 0;
			for (int i = 0; i < s->full_contestant_count; i++) {
				if (current_lives[i] > 0) {
					alive_ids[current_alive_count] = s->base_field[i]->id;
					current_alive_count++;
				}
			}

			// score simulation
			for (int cont = 0; cont < current_alive_count; cont++) {
				int c_id = alive_ids[cont];

				round_scores[c_id] = -99999;

				// simulate performance by rolling gaussian rng for each response the player has
				// pick the highest as per standard DRP/TRP rules
				for (int resp = 0; resp < drp_level[c_id]; resp++) {

					double response_score = multiplied_box_muller(
						s->base_field[c_id]->average, s->base_field[c_id]->stdev,
						0.5, SCORE_RENORMALIZATION);
					
					if (response_score > round_scores[c_id]) round_scores[c_id] = response_score;
				}
			}

			// sort alive contestants based on their simulated performance
			contestant_qsort(alive_ids, round_scores, 0, current_alive_count-1);

			// calculate zone thresholds
			int round_prize_threshold = round(current_alive_count * s->prize_rate);
			int round_elim_threshold = elim_threshold(current_alive_count, s->elim_rate);

			// remove a life from/kill the ones in the elim zone, as well as update the alive count
			for (int r = current_alive_count - 1; r >= round_elim_threshold; r--) {
				int c_id = alive_ids[r];
				current_lives[c_id]--;

				if (current_lives[c_id] == 0) {
					final_ranks[c_id] = current_alive_count;
					current_alive_count--;
				}
			}

			// select DRP level based on next round's contestant count and UDRP and UTRP thresholds
			int universal_drp_level = (current_alive_count <= s->utrp_threshold) ? 3 : (
				((current_alive_count <= s->udrp_threshold)) ? 2 : 1);

			// wipe slate clean for next round's DRP data
			for (int i = 0; i < s->full_contestant_count; i++) {
				drp_level[i] = universal_drp_level;
			}

			// apply a prize to the people in the prize region
			for (int r = 0; r < round_prize_threshold; r++) {
				int c_id = alive_ids[r];

				if (s->prize_type) { // prize type 1 -> lives
					// add life if they're not already at the cap
					if (s->life_cap <= 0 || current_lives[c_id] < s->life_cap) {
						current_lives[c_id]++;
					}

				} else { // prize type 0 -> extra response
					drp_level[c_id]++;
				}
			}

			simulated_round_count++;

		} while (current_alive_count > 1);

		// only one contestant left; give them the winner rank
		final_ranks[alive_ids[0]] = 1;

		// log all contestant ranks in the aggregate results
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