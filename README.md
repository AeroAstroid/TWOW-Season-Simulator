# Dark's TWOW Season Simulator
A simple, customizable C program that simulates seasons of TWOW given normal distribution data for each contestant.

## Customization

### Contestant Data
The program reads contestant performance data from the **`data/distributions.txt`** file. The file is a tab-separated values table where each row is a contestant, and the columns correspond to **name**, **average**, **standard deviation**, and **lives**. For simulating seasons that don't have a lives system, you can omit the last column, and contestants will be assigned a life count of 1 automatically.

For highest accuracy, it is recommended to use **standardized ranks** as the performance data, however **normalized ranks** are an acceptable substitute.

The sample file in the repo contains data for all EWOW contestants as of EWOW 3B.

### Game Data

The program reads game variables from the **`data/game.txt`** file. The file contains five customizable variables:

* **`ELIM_RATE`:** the percentage of contestants that get eliminated/lose a life per round, expressed as a decimal value;
* **`ENSURE_LESS_THAN_HALF`:** a boolean value (0 or 1). If set to 1, the elimination region will always be less than half the contestant field (except in finales), such as in EWOW;
* **`LIFE_GAIN_RATE`:** the percentage of contestants that gain a life per round, expressed as a decimal value;
* **`SCORE_RENORMALIZATION`:** the strength of the score renormalization. _It is highly recommended to not change this value unless you know what you're doing;_
* **`THRESHOLD_POINTS`:** a list of contestant counts for which the program will output contestant odds (e.g. Top 5000, Top 1000, Top 500, etc).

The sample file in the repo contains values applicable to EWOW as of EWOW 3B.

### Program Data

The program reads program parameters from the **`data/program.txt`** file. The file contains two customizable variables:

* **`ITERATIONS`:** the amount of seasons the program will simulate before stopping;
* **`THREADS`:** the amount of threads to run simulations with. It is not recommended to go above 4.

## Usage

This program comes with pre-compiled executables for Windows and Linux. To run them, clone/download this repo to some folder on your computer, open a terminal of your choice in that folder, and run the command **`bin/simulator`**.

Depending on your architecture, the executables may be incompatible with your system, and you'll need to compile the code yourself. To do so, you'll need the `gcc` compiler. To compile, run the command **`gcc main.c module/*.c -o bin/simulator -Ofast -lm -Wno-unused-result`**. After that, you can use the run command from the last paragraph.

If you have `make` installed on your system, you can also just use the commands **`make all`** to compile the code and **`make run`** to run it.

Changing any of the three customizable data files does not require recompilation; you can simply modify them and run the executable again.
