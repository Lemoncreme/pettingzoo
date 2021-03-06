import os
import shutil
from game import Game
import numpy as np
from joblib import dump
from tqdm import tqdm
from copy import deepcopy

class RunLogger():
    r"""A class to monitor and log the progress of a run.

    Args:
    ---
    output_dir:  A directory to output run progress and agents

    Features:
    ---
    Will dump stats of each generation to a run_log.txt file in the output directory. \
    Will dump top N agents of each generation. \
    Can be used to print out stats about each generation
    """
    def __init__(self, output_dir):
        self.output_dir = output_dir

        if not os.path.exists(output_dir):
            os.mkdir(output_dir)

        self.run_log_file = None
        self.setup_output()

        self.n_gens = 0
        self.overall_best_fitness = 0

    def setup_output(self):
        self.output_dir = os.path.expanduser(self.output_dir)
        self.output_dir = os.path.abspath(self.output_dir)

        try:
            os.mkdir(self.output_dir)
        except FileExistsError:
            resp = input(f"Output dir '{self.output_dir}' exists!\nWould you like to delete? Y / N: ")
            if resp.lower() != "y":
                print("Please provide a new output path.")
                exit(-1)
            
            shutil.rmtree(self.output_dir)
            os.mkdir(self.output_dir)

        self.run_log_file = open(os.path.join(self.output_dir, "run_log.txt"), 'w')

    def log_generation(self, agents, fitnesses, death_types, game_args):
        avg_fit = np.mean(fitnesses)
        min_fit = np.min(fitnesses)
        max_fit = np.max(fitnesses)

        # Get counts for each kind of death type
        deaths = {Game.PLAYER_COMPLETE: 0, Game.PLAYER_DEAD: 0, Game.PLAYER_TIMEOUT: 0}
        death_types, counts = np.unique(death_types, return_counts=True)
        for death_type, count in zip(death_types, counts):
            deaths[death_type] = count

        self.run_log_file.write(
            f"{min_fit:0.2f}, {max_fit:0.2f}, {avg_fit:0.2f}, {deaths[Game.PLAYER_DEAD]}, {deaths[Game.PLAYER_COMPLETE]}, {deaths[Game.PLAYER_TIMEOUT]}\n"
        )
        self.run_log_file.flush()

        # Save out best from this generation
        best = np.argmax(fitnesses)
        dump([agents[best], game_args], os.path.join(self.output_dir, f"{self.n_gens}_{fitnesses[best]:0.2f}.joblib"))

        # Overwrite overall best if better
        if max_fit > self.overall_best_fitness:
            self.overall_best_fitness = max_fit
            dump([agents[best], game_args],
                os.path.join(self.output_dir,
                "best.joblib"))

        self.n_gens += 1

        print_stats(min_fit, max_fit, avg_fit, deaths, game_args)
    
    def copy_topn(self, agents, fitnesses, topn):
        if topn == 0:
            return None

        topn_ind = list(np.argpartition(fitnesses, -topn)[-topn:])

        topn_agents = []
        for ind in topn_ind:
            agent = agents[ind]
            topn_agents.append(deepcopy(agent))

        return topn_agents

def print_stats(min_fit, max_fit, avg_fit, deaths, game_args):
    tqdm.write("#" * 30)
    tqdm.write(f"Seed: {game_args['seed']}")
    tqdm.write(f"Avg: {avg_fit:0.2f}")
    tqdm.write(f"Min: {min_fit:0.2f}")
    tqdm.write(f"Max: {max_fit:0.2f}")
    tqdm.write("")
    tqdm.write(f"Died:      {deaths[Game.PLAYER_DEAD]}")
    tqdm.write(f"Completed: {deaths[Game.PLAYER_COMPLETE]}")
    tqdm.write(f"Timed out: {deaths[Game.PLAYER_TIMEOUT]}")