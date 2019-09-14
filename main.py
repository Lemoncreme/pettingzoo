from game.core.logic import Game
from game.rendering.renderer import Renderer
from time import time

def main():
    seed = int(time())
    renderer = Renderer()
    game = Game(num_chunks=50, seed=seed)

    renderer.new_game_setup(game)

    while renderer.running:
        keys = renderer.get_input()

        game.update(keys)

        if game.game_over:
            seed = int(time())
            game = Game(num_chunks=50, seed=seed)
            renderer.new_game_setup(game)
            continue
    
        renderer.draw_state(game, keys)

if __name__ == "__main__":
    main()