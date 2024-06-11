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
        for (int i = 0; i < s->full_contestant_count; i++) {
            current_lives[i] = s->base_field[i]->start_lives;
        }

        // init final ranks array
        int final_ranks[s->full_contestant_count];
        for (int i = 0; i < s->full_contestant_count; i++) {
            final_ranks[i] = -1;
        }

        int alive_ids[s->full_contestant_count]; // list of IDs for every alive contestant
        double round_scores[s->full_contestant_count]; // simulated score for every alive contestant

        int alive_contestants;
        
        do {
            alive_contestants = 0;
            for (int i = 0; i < s->full_contestant_count; i++) {
                if (current_lives[i] > 0) {
                    alive_ids[alive_contestants] = s->base_field[i]->id;
                    alive_contestants++;
                }
            }

            for (int cont = 0; cont < alive_contestants; cont++) {
                int c_id = alive_ids[cont];
                double sim_score = box_muller(s->base_field[c_id]->average, s->base_field[c_id]->stdev);

                // normalization factor
                if (s->score_renormalization) {
                    sim_score *= box_muller(0.5, s->score_renormalization);
                }

                round_scores[c_id] = sim_score;
            }

            // sort alive contestants based on their simulated performance
            contestant_qsort(alive_ids, round_scores, 0, alive_contestants-1);

            // remove a life from the ones in the elim zone
            int round_elim_threshold = elim_threshold(alive_contestants, s->elim_rate, s->ensure_less_than_half);
            for (int r = alive_contestants - 1; r >= round_elim_threshold; r--) {
                int c_id = alive_ids[r];
                current_lives[c_id]--;

                if (current_lives[c_id] == 0) {
                    final_ranks[c_id] = alive_contestants;
                    alive_contestants--;
                }
            }

            // add a life from the ones in the life gain zone
            int round_life_gain_threshold = round(alive_contestants * s->life_gain_rate);
            for (int r = 0; r < round_life_gain_threshold; r++) {
                int c_id = alive_ids[r];
                current_lives[c_id]++;
            }

        } while (alive_contestants > 1);

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
    }
}