#ifndef DEFS_H_
#define DEFS_H_

// Level parameters
#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 128
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define LEVEL_PIXEL_WIDTH LEVEL_WIDTH * TILE_WIDTH
#define LEVEL_PIXEL_HEIGHT LEVEL_HEIGHT * TILE_HEIGHT
#define LEVEL_SIZE LEVEL_HEIGHT * LEVEL_WIDTH
#define MAX_ENEMIES 32
#define SPAWN_X 2
#define SPAWN_Y 10

// Level generation parameters
#define GROUND_HEIGHT LEVEL_HEIGHT - LEVEL_HEIGHT / 4

// Sprite parameters
#define T_EMPTY 0
#define T_GRASS 1
#define T_DIRT 2
#define T_BRICKS 3
#define T_SPIKES 4

// Player physics parameters 
#define V_X 6
#define V_JUMP 8
#define INTERTA 1.5
#define GRAVITY 0.3

#endif