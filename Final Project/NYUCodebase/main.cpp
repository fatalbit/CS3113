/* Eric Liang
	CS3113 Final Project
	NetID: ezl207
*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "ShaderProgram.h"
#include "Entity.h"
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "Title.h"
#include "Game.h"
#include "Over.h"
#include "utils.h"
#include "config.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define RAD_CONV (3.1415926 / 180.0)

SDL_Window* displayWindow;
void Setup(SDL_Window** displayWindow, Matrix* projectionMatrix);

enum level {LEVEL_ONE, LEVEL_TWO, LEVEL_THREE};

int state = STATE_TITLE;

unsigned int LEVEL_X = 0;
unsigned int LEVEL_Y = 0;
int LEVEL_X_END = -1;
int LEVEL_Y_END = 30;

std::vector<int> solid{ 121, 122, 123, 124, 125, 128, 129, 151, 152, 153, 154, 155, 156, 157, 158, 159,
						272, 242,
						512, 482

	};
std::vector<int> enemy_markers{ 899 };
std::vector<int> flag{ 310 };
Mix_Music* shoot;
int points = 0;
int cur_level = LEVEL_ONE;
int main(int argc, char *argv[])
{
	SDL_Event event;
	bool done = false;
	float lastFrameTicks = 0.0f;
	float ball_angle = 0.0f;
	srand(time(NULL));

	Matrix projectionMatrix;
	Matrix viewMatrix;
	Setup(&displayWindow, &projectionMatrix);

	std::vector<Entity*> entities;
	std::vector<Entity*> markers;
	std::string text;
	Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, CHANNELS, AUD_BUF_SIZE);
  shoot = Mix_LoadMUS("shoot.wav")
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program.programID);

	GLuint plat_sprites = LoadTexture("spritesheet_rgba.png", GL_RGBA);
	GLuint text_sprites = LoadTexture("font1.png", GL_RGBA);
	GLuint b_fire = LoadTexture("blue_fireball.png", GL_RGBA);
	GLuint r_fire = LoadTexture("red_fireball.png", GL_RGBA);

	Entity missle(b_fire, 1536.0f, 394.0f, 3, 2, 0, 0.0f, 512.0f, 197.0f, 0.5f, 1);
	missle.isflying = true;
	missle.add_animation("moving", { 0, 1, 2, 3, 4, 5 });
	missle.set_animation("moving");
	missle.entityType = ENTITY_PROJECTILE;

	Entity player(plat_sprites, 692.0f, 692.0f, 30, 30, 2.0f, 28.0f, 21.0f, 21.0f, 0.5f, 1);

	player.add_animation("idle", { 19 });
	player.add_animation("moving", { 28, 29 });
	player.add_animation("jumping", {20});
	player.set_animation("idle");
	player.damage = 1;

	player.facingRight = true;
	player.entityType = ENTITY_PLAYER;

	Entity powerup(plat_sprites, 692.0f, 692.0f, 30, 30, 2.0f, 467, 21.0f, 21.0f, 0.5f, 1);
	powerup.isAlive = false;
	powerup.entityType = ENTITY_POWERUP;
	powerup.add_animation("idle", { 467 });
	powerup.set_animation("idle");

	Entity flag(plat_sprites, 692.0f, 692.0f, 30, 30, 2.0f, 340, 21.0f, 21.0f, 0.5f, 1);
	flag.entityType = ENTITY_POWERUP;
	flag.add_animation("idle", {310, 340});
	flag.set_animation("idle");

	unsigned short** level = nullptr;
	read_level(level, LEVEL_X, LEVEL_Y, entities, player, flag, plat_sprites, "level1.txt");

	while (!done) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		float fixedElapsed = elapsed;
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		switch (state){
		case(STATE_TITLE) :
			viewMatrix.identity();
			TitleProcessEvents(&event,state, done);
			TitleRender(&program, displayWindow, text_sprites);
			break;
		case(STATE_GAME) :
			viewMatrix.identity();
			viewMatrix.Translate(-player.x, -player.y, 0);
			GameProcessEvents(&event, &done, elapsed, entities, missle, player, shoot);
			if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS){
				fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
			}
			while (fixedElapsed >= FIXED_TIMESTEP){
				fixedElapsed -= FIXED_TIMESTEP;
				GameUpdate(FIXED_TIMESTEP, plat_sprites, entities, solid, enemy_markers, flag, missle, LEVEL_X, LEVEL_Y, level, points, player, powerup, state, cur_level);
			}
			GameUpdate(fixedElapsed,plat_sprites, entities, solid, enemy_markers, flag, missle, LEVEL_X, LEVEL_Y , level, points, player, powerup, state, cur_level);
			text = "Points:" + std::to_string(points);
			if (player.damage == 2){
				missle.sprite.textureID = r_fire;
			}
			GameRender(&program, displayWindow, entities, flag, text , missle, LEVEL_X, LEVEL_Y, level, text_sprites, plat_sprites, player, powerup);
			break;
		case(STATE_OVER) :
			viewMatrix.identity();
			missle.sprite.textureID = b_fire;
			Reset_Level(plat_sprites, player, entities, flag, LEVEL_X, LEVEL_Y, level, "level1.txt");
			OverProcessEvents(&event, state, done);
			OverRender(&program, displayWindow, text_sprites);
			break;
		default:
			break;
		}
	}

	for (size_t i = 0; i < entities.size(); ++i){
		delete entities[i];
	}

	for (size_t i = 0; i < LEVEL_Y; ++i){
			delete[] level[i];
	}
	delete[] level;
	SDL_Quit();
	return 0;
}

void Setup(SDL_Window** displayWindow,Matrix* projectionMatrix){
	SDL_Init(SDL_INIT_VIDEO);
	*displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(*displayWindow);
	SDL_GL_MakeCurrent(*displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 720);
	projectionMatrix->setOrthoProjection(-ORTHO_X, ORTHO_X, -ORTHO_Y, ORTHO_Y, -1.0f, 1.0f);
}
