#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "module/contestant.h"
#include "module/calculation.h"
#include "module/simulation.h"
#include "module/input.h"

int main() {
	// seed the random number generator
	seed_xoshiro();

	int ensure_less_half, iteration_target, thread_count, contestant_count, game_stages, life_decay_timer, life_cap;
	double elim_rate, life_gain_rate, score_renorm, ld_elim_rate, ld_life_gain_rate;
	int* threshold_points;

	read_game_rules("data/game.txt",
		&elim_rate, &ensure_less_half, &life_gain_rate, &life_cap,
		&life_decay_timer, &ld_elim_rate, &ld_life_gain_rate,
		&score_renorm, &threshold_points, &game_stages);
	
	read_program_params("data/program.txt", &iteration_target, &thread_count);

	Contestant** base_field = read_all_contestants("data/distributions.txt", &contestant_count);

	pthread_t thread_list[thread_count];

	// prepare the simulation results counter
	int aggregate_results[contestant_count*game_stages];
	for (int i = 0; i < contestant_count*game_stages; i++) aggregate_results[i] = 0;

	// create struct of simulation parameters that gets passed onto each thread
	SimulationInfo* sim_info = create_sim_info(iteration_target, base_field, contestant_count, threshold_points,
		game_stages, aggregate_results, elim_rate, ensure_less_half, life_gain_rate, life_cap, life_decay_timer,
		ld_elim_rate, ld_life_gain_rate, score_renorm);

	printf("Running %d season simulations on %d thread(s)\n", iteration_target, thread_count);
	
	// start timer
	struct timespec start, finish, delta;
	clock_gettime(CLOCK_REALTIME, &start);

	for (int t = 0; t < thread_count; t++) {
		pthread_create(&thread_list[t], NULL, simulation_thread, (void*)sim_info);
	}

	for (int t = 0; t < thread_count; t++) {
		pthread_join(thread_list[t], NULL);
	}

	// end timer
	clock_gettime(CLOCK_REALTIME, &finish);
	sub_timespec(start, finish, &delta);

	double elapsed = delta.tv_sec + (double)delta.tv_nsec/1000000000L;
	printf("Finished running simulations in %.4lf seconds.\n", elapsed);
	printf("Average season took %.3lf more rounds to wrap up.\n", (double)sim_info->total_rounds_taken/iteration_target);

	printf("Writing results to output file...\n");

	FILE* output = fopen("output.txt", "w");

	if (output == NULL) {
		printf("Error opening output file!");
		exit(-1);
	}

	fprintf(output, "Name\t");
	for (int s = 0; s < game_stages; s++) {
		fprintf(output, "Top %d\t", threshold_points[s]);
	}
	fprintf(output, "\n");

	for (int c = 0; c < contestant_count; c++) {
		fprintf(output, "%s\t", base_field[c]->name);
		for (int s = 0; s < game_stages; s++) {
			fprintf(output, "%lf\t", (double)aggregate_results[c*game_stages+s]/iteration_target);
		}

		fprintf(output, "\n");

		free(base_field[c]);
	}

	fclose(output);

	free(base_field);
	free(sim_info);
	free(threshold_points);

	printf("Finished program! Results are in the [output.txt] file.\n");

	return 0;
}