#ifndef _GAME
	#define _GAME
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Entity.h"
#include "DrawSpriteText.h"


void GameProcessEvents(SDL_Event* event, bool* done, float elapsed, std::vector<Entity*> entities, Entity& missle, Entity& player, Mix_Music* shoot);
void GameUpdate(float elapsed, GLuint sprites, std::vector<Entity*>& entities, std::vector<int> solid, std::vector<int> enemy_markers, Entity& flag, Entity& missle, unsigned& LEVEL_X, unsigned& LEVEL_Y, unsigned short**& level, int& points, Entity& player, Entity& powerup, int& state, int& cur_level);
void GameRender(ShaderProgram* program, SDL_Window*	displayWindow,
				std::vector<Entity*> entities, Entity& flag,
				std::string text, Entity missle, unsigned LEVEL_X, 
				unsigned LEVEL_Y,unsigned short** level, GLuint text_sprites,
				GLuint level_sprites, Entity& player, Entity& powerup );

#endif
