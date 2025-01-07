# Dark's TWOW Season Simulator (EWOW Version)
A simple, customizable C program that simulates seasons of TWOW given normal distribution data for each contestant.

**This branch contains a version of the simulator tuned specifically for simulating EWOW. For a more generalized version of this simulator, [check the `main` branch of this repository.](https://github.com/AeroAstroid/TWOW-Season-Simulator/tree/main)**

## Customization

### Contestant Data
The program reads contestant performance data from the **`data/distributions.txt`** file. The file is a tab-separated values table where each row is a contestant, and the columns correspond to **name**, **average**, **standard deviation**, **current response count**, and **lives**. The last two columns are both optional, and both will default to 1 if omitted.

For highest accuracy, it is recommended to use **standardized ranks** as the performance data, however **normalized ranks** are an acceptable substitute.

The sample file in the repo contains data for all **EWOW** contestants as of round 10.

### Game Data

The program reads game variables from the **`data/game.txt`** file. The file contains 12 customizable variables:

* **`ELIM_RATE`:** the percentage of contestants that get eliminated/lose a life per round, expressed as a decimal value;
* **`PRIZE_RATE`:** the percentage of contestants that prize in each round, expressed as a decimal value. By default, prizing gives an extra life if life decay isn't active, and gives an extra response if it is;
* **`LIFE_CAP`:** the amount of lives at which a contestant can't gain any extra lives. If a contestant at the life cap places in the prize region, their life count does not change. Set this to 0 or -1 to have no life cap;
* **`LIFE_DECAY_TIMER`:** the amount of simulated rounds until life decay kicks in. Life decay makes it so after results for each round, every contestant who's above the life floor (defined below) has their life count decayed by 1, such as in EWOW. Set this to 0 to make life decay start instantly, set this to -1 to have no life decay;
* **`LIFE_DECAY_STARTING_FLOOR`:** the floor for the first phase of life decay, a.k.a. the amount of lives contestants get decayed to. Once every contestant is at or below the life floor, if the life floor is not already 1, it gets lowered to 1 and the second phase of life decay begins;
* **`LD_PHASE_1_ELIM_RATE`:** once/if phase 1 of life decay starts, the elim rate variable will be set to this value until phase 2 of life decay starts;
* **`LD_PHASE_1_PRIZE_RATE`:** once/if phase 1 of life decay starts, the prize rate variable will be set to this value until phase 2 of life decay starts;
* **`LD_PHASE_2_ELIM_RATE`:** once/if phase 2 of life decay starts, the elim rate variable will be set to this value for the rest of the season;
* **`LD_PHASE_2_PRIZE_RATE`:** once/if phase 2 of life decay starts, the prize rate variable will be set to this value for the rest of the season;
* **`UDRP_THRESHOLD`:** the contestant count at which every contestant starts to be granted 2 responses by default;
* **`UTRP_THRESHOLD`:** the contestant count at which every contestant starts to be granted 3 responses by default (usually reserved for finales);
* **`THRESHOLD_POINTS`:** a list of contestant counts for which the program will output contestant odds (e.g. Top 5000, Top 1000, Top 500, etc).

The sample file in the repo contains values applicable to **EWOW**'s game state as of round 10.

### Program Data

The program reads program parameters from the **`data/program.txt`** file. The file contains 2 customizable variables:

* **`ITERATIONS`:** the amount of seasons the program will simulate before stopping;
* **`THREADS`:** the amount of threads to run simulations with. It is not recommended to go above 4.

## Usage

This program comes with pre-compiled executables for Windows and Linux. To run them, clone/download this repo to some folder on your computer, open a terminal of your choice in that folder, and run the command **`bin/simulator`**.

Depending on your system's architecture, the executables may be incompatible with your system, and you'll need to compile the code yourself. To do so, you'll need the `gcc` compiler. If you don't already have it installed, [check out this tutorial.](https://www.youtube.com/watch?v=k6juv3mIr9o) To compile the code, run the command **`gcc main.c module/*.c -o bin/simulator -O1 -lm -Wno-unused-result`**. After that, you can use the run command from the last paragraph.

If you have `make` installed on your system, you can also just use the commands **`make all`** to compile the code and **`make run`** to run it.

Changing any of the three customizable data files does not require recompilation; you can simply modify them and run the executable again.
