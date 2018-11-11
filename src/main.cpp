#include <SFML/Graphics.hpp>
#include <defs.hpp>
#include <rendering.hpp>
#include <gamelogic.hpp>
#include <sys/stat.h>
#include <neural_network.hpp>
#include <time.h>

int main()
{
	int draw_overlay = 0;
	int input[BUTTON_COUNT] = {0};
	int ret = 0;
    sf::RenderWindow window(sf::VideoMode(800, 600), "PettingZoo");
	sf::Time frame_time;
	sf::Clock clock;
	sf::Color bg_color(135, 206, 235);
	struct Game game;
	struct Player player;
	uint8_t curbuttons;
	uint seed;
	size_t nbytes;
	uint8_t *bytes = NULL;
	uint8_t *buttons = NULL;
	uint8_t *chrom = NULL;

	//Get size of file
	struct stat st;
	stat("output.bin", &st);
	nbytes = st.st_size;
	bytes = (uint8_t *)malloc(nbytes);

	//Read file, extract information
	FILE *infile = fopen("output.bin", "r");
	fread(buttons, sizeof(uint8_t), MAX_FRAMES, infile);
	fclose(infile);
	seed = extract_from_bytes(bytes, chrom, buttons);

	window.setKeyRepeatEnabled(false);
	window.setVerticalSyncEnabled(true);

	seed = time(NULL);

	game_setup(&game, &player, seed);
	render_load_assets();
	render_gen_map(game);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			int is_pressed = (event.type == sf::Event::KeyPressed);
			switch(event.key.code) {
				// Jump
				case sf::Keyboard::Up:
				case sf::Keyboard::Space:
				case sf::Keyboard::W:
					input[BUTTON_JUMP] = is_pressed;
					break;
				// Left
				case sf::Keyboard::Left:
				case sf::Keyboard::A:
					input[BUTTON_LEFT] = is_pressed;
					break;
				// Right
				case sf::Keyboard::Right:
				case sf::Keyboard::D:
					input[BUTTON_RIGHT] = is_pressed;
					break;
				case sf::Keyboard::Escape:
					return 0;
				case sf::Keyboard::O:
					draw_overlay ^= 1 * is_pressed;
					break;
				// Reset game state
				case sf::Keyboard::R:
					if (is_pressed) {
						seed = time(NULL);
						game_setup(&game, &player, seed);
						render_gen_map(game);
					}
					break;
				default:
					break;
			}

			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::Resized) {
				render_scale_window(window, event);
			}
		}

		//Get buttons
		curbuttons = buttons[game.frame];
		input[BUTTON_RIGHT] = curbuttons & 0x1;
		input[BUTTON_LEFT] = curbuttons & 0x2;
		input[BUTTON_JUMP] = curbuttons & 0x4;

		//Update game state
		ret = game_update(&game, &player, input);
		if (ret == PLAYER_DEAD) {
		    printf("PLAYER DEAD\n SCORE: %d\n FITNESS: %d\n", player.score, player.fitness);
			seed = time(NULL);
			game_setup(&game, &player, seed);
			render_gen_map(game);
		} else if (ret == REDRAW) {
			render_gen_map(game);
		}

		// Update camera
		render_handle_camera(window, player);

		//Clear the screen
		window.clear(bg_color);

		//Draw background, tiles, and entities
		render_draw_state(window, game, player);

		//Increment frame
		game.frame += 1;

		//Draw debug overlay + fps
		frame_time = clock.getElapsedTime();
		clock.restart();
		if (draw_overlay) {
			render_debug_overlay(window, game, player, frame_time);
		}

		// Score and time
		render_hud(window, player, input);

		window.display();
	}

	return 0;
}
