#ifndef _CONFIG
	#define _CONFIG
#define SPRITE_X 30
#define SPRITE_Y 30
#define SPRITE_PADDING 2
#define SPRITE_WIDTH 21
#define SPRITE_SIZE 0.50f
#define TILE_SIZE 0.50f
#define FIXED_TIMESTEP 0.00694444f
#define MAX_TIMESTEPS 14.4
#define ANIMATION_SPEED 2.0f
#define FREQUENCY 44100
#define CHANNELS 2
#define AUD_BUF_SIZE 4096
#define ORTHO_X 7.08f
#define ORTHO_Y 4.0f
#define FRICTION_X 1.00f
#define GRAVITY -9.8f
#define DEBUG 1
#define TITLE "Platformer Demo"


enum GameState { STATE_TITLE, STATE_GAME, STATE_OVER, STATE_END };
enum EntType { BEE, MOUSE, BAT, PLAYER , FLAG};
#endif