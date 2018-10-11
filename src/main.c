#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <rendering.h>
#include <defs.h>
#include <math.h>

float zoom = 2.0;

int rescale_window(sfView *view, sfEvent event);

int main(int argc, char **argv)
{
	int opt, draw_overlay;
	int input[BUTTON_COUNT] = {0};

	sfVideoMode mode = {800, 600, 32};
	sfRenderWindow* window;
	sfVector2f moveby = {0, 0};
	sfEvent event;
	sfTime time;
	sfClock *clock;
	sfView *view;
	sfColor background = {230, 230, 230, 1};
	draw_overlay = 0;

	// Create the clock
	clock = sfClock_create();

	// Create the main window
	window = sfRenderWindow_create(mode, "Petting Zoo", sfResize | sfClose, NULL);
	if (!window)
		return -1;
	sfRenderWindow_setKeyRepeatEnabled(window, sfFalse);

	// Vsync 
	sfRenderWindow_setVerticalSyncEnabled(window, sfTrue);

	//Load assets
	game_load_assets(draw_overlay);

	//Generate game
	game_setup();

	// Start the game loop
	view = sfView_copy(sfRenderWindow_getView(window));
	while (sfRenderWindow_isOpen(window))
	{
		// Process events
		while (sfRenderWindow_pollEvent(window, &event))
		{
			// Close window
			if (event.type == sfEvtClosed) {
				sfRenderWindow_close(window);
			} else if (event.type == sfEvtResized) {
				rescale_window(view, event);
			} else if (event.type == sfEvtKeyPressed) {
				if (event.key.code == sfKeyUp) {
					input[BUTTON_JUMP] = 1;
				} else if (event.key.code == sfKeyLeft) {
					input[BUTTON_LEFT] = 1;
				} else if (event.key.code == sfKeyRight) {
					input[BUTTON_RIGHT] = 1;
				} else if (event.key.code == sfKeyEscape) {
					goto exit;
				} else if (event.key.code == sfKeyO) {
					draw_overlay ^= 1;
				}
			} else if (event.type == sfEvtKeyReleased) {
				if (event.key.code == sfKeyLeft) {
					input[BUTTON_LEFT] = 0;
				} else if (event.key.code == sfKeyRight) {
					input[BUTTON_RIGHT] = 0;
				} else if (event.key.code == sfKeyUp) {
					input[BUTTON_JUMP] = 0;
				}
			}
		}
		//Frametime
		time = sfClock_getElapsedTime(clock);

		// Restart the clock
		sfClock_restart(clock);

		//Update game state
		game_update(window, view, input);
		
		// Update camera
		handle_camera(window, view);

		//Clear the screen
		sfRenderWindow_clear(window, background);

		//Draw background
		game_draw_other(window, view);

		//Draw the tiles and entities
		game_draw_tiles(window, view, draw_overlay);
		game_draw_entities(window, view);

		//Draw coords if needed
		if (draw_overlay) {
			game_draw_overlay_text(window, view, time);
		}

		// Update the window
		sfRenderWindow_display(window);
	}

exit:
	// Cleanup resources
	sfRenderWindow_destroy(window);
	sfClock_destroy(clock);
	sfView_destroy(view);
	
	return 0;
}

int rescale_window(sfView *view, sfEvent event)
{
	sfVector2f win_size = {event.size.width, event.size.height};
	sfVector2f win_center = sfView_getCenter(view);

	//Auto zoom depending on window size
	zoom = win_size.y / (LEVEL_PIXEL_HEIGHT);
	zoom = round(win_size.y / (LEVEL_PIXEL_HEIGHT));
	zoom = zoom < 1 ? 1 : zoom;
	win_size.x /= zoom;
	win_size.y /= zoom;
	
	sfView_setSize(view, win_size);

	return 0;
}
