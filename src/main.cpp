#include <SFML/Graphics.hpp>
#include <defs.hpp>
#include <rendering.hpp>
#include <gamelogic.hpp>

int main()
{
	int draw_overlay = 0;
	int input[BUTTON_COUNT] = {0};
	int ret = 0;
    sf::RenderWindow window(sf::VideoMode(800, 600), "PettingZoo");
	sf::Time time;
	sf::Clock clock;
	sf::Color bg_color(135, 206, 235);

	window.setKeyRepeatEnabled(false);
	window.setVerticalSyncEnabled(true);

	game_setup();
	render_load_assets();

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
						game_setup();
						render_regen_map();
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

		//Update game state
		ret = game_update(input);
		if (ret == PLAYER_DEAD) {
			game_setup();
			render_regen_map();
		} else if (ret == REDRAW) {
			render_regen_map();
		}

		// Update camera
		render_handle_camera(window);

		//Clear the screen
		window.clear(bg_color);

		//Draw background, tiles, and entities
		render_draw_state(window);

		//Draw debug overlay + fps
		time = clock.getElapsedTime();
		clock.restart();
		if (draw_overlay) {
			render_debug_overlay(window, time);
		}

		// Score and time
		render_hud(window, input);

		window.display();
	}

	return 0;
}
