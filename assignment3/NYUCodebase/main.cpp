/* Eric Liang
	CS3113 Game Programming Assignment 3
	NetID: ezl207
*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "ShaderProgram.h"
#include "Entity.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define RAD_CONV (3.1415926 / 180.0)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;
GLuint LoadTexture(const char *image_path, DWORD imageType);
void Setup(SDL_Window** displayWindow, Matrix* projectionMatrix);
void ProcessEvents(SDL_Event* event, bool* done, float elapsed,Entity& bullet, Entity& player);
void Render(ShaderProgram* program, std::vector<Entity*>& entities, Entity& bullet, Entity& explosion, Entity& player);
void Update(float elapsed, std::vector<Entity*>& entities, Entity& bullet, Entity& explosion, Entity& player);

int state;

const float friction_x = 0.1f;
const float gravity = 0.0f;

enum GameState {STATE_TITLE, STATE_GAME};

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
	
	GLuint space_sheet = LoadTexture("sheet.png", GL_RGBA);
	Entity* newEnt;
	for (size_t x = 0; x < 11; ++x){
		for (size_t y = 0; y < 4; ++y){
			
			switch (y){
			case 0:
				newEnt = new Entity(space_sheet, 1024.0f, 423.0f, 728.0f, 93.0f, 84.0f, 0.5f);

				break;
			case 1:
				newEnt = new Entity(space_sheet, 1024.0f, 425.0f, 468.0f, 93.0f, 84.0f, 0.5f);
				break;
			default:
				newEnt = new Entity(space_sheet, 1024.0f, 425.0f, 384.0f, 93.0f, 84.0f, 0.5f);
				break;
			}
			
			newEnt->x = (-6.22 + newEnt->width/2) + (x * (newEnt->width + 0.1));
			newEnt->y = (3.5 - newEnt->height/2 ) - (y * (newEnt->height + 0.1));
			entities.push_back(newEnt);
		}
	}
	
	Entity player(space_sheet, 1024.0f, 211.0f, 941.0f, 99.0f, 75.0f, 0.5);
	player.y = -3.5 + 0.1 + player.height/2;

	Entity bullet(space_sheet, 1024.0f, 843.0f, 789.0f, 13.0f, 57.0f, 0.5);
	bullet.x = 7.0;
	
	Entity explosion(space_sheet, 1024.0f, 737.0f, 613.0f, 37.0f, 37.0f, 0.5);
	explosion.x = 7.0;

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glUseProgram(program.programID);
	
	while (!done) {
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		ProcessEvents(&event, &done, elapsed, bullet, player);

		float fixedElapsed = elapsed;

		if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS){
			fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
		}
		while (fixedElapsed >= FIXED_TIMESTEP){
			fixedElapsed -= FIXED_TIMESTEP;
			Update(FIXED_TIMESTEP, entities, explosion, bullet, player);
		}
		Update(fixedElapsed, entities,bullet, explosion, player);

		glClear(GL_COLOR_BUFFER_BIT);

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		Render(&program, entities, bullet, explosion, player);
		
	}
	for (size_t i = 0; i < entities.size(); ++i){
		delete entities[i];
	}

	SDL_Quit();
	return 0;
}

GLuint LoadTexture(const char *image_path, DWORD imageType) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, imageType, surface->w, surface->h, 0, imageType,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
	return textureID;
}

void Setup(SDL_Window** displayWindow,Matrix* projectionMatrix){
	SDL_Init(SDL_INIT_VIDEO);
	
	*displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(*displayWindow);
	SDL_GL_MakeCurrent(*displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 360);
	projectionMatrix->setOrthoProjection(-6.22f, 6.22f, -3.5f, 3.5f, -1.0f, 1.0f);
}

void ProcessEvents(SDL_Event* event,bool* done, float elapsed, Entity& bullet, Entity& player){
	while (SDL_PollEvent(event)) {
		//Event Loop
		if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
			*done = true;
		}
		else if (event->type == SDL_KEYDOWN){
			if (event->key.keysym.scancode == SDL_SCANCODE_D){
				player.velocity_x = 2;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_A){
				player.velocity_x = -2;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_SPACE){
				if (
					6.22 <= bullet.x ||
					bullet.x <= -6.22 ||
					3.5 <= bullet.y ||
					bullet.y <= -3.5
					)
				{
					bullet.x = player.x;
					bullet.y = player.y + bullet.height;
					bullet.velocity_y = 4.0;
				}
			}
		}
		else if (event->type == SDL_KEYUP){
			if (event->key.keysym.scancode == SDL_SCANCODE_D){
				player.velocity_x = 0;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_A){
				player.velocity_x = 0;
			}
		}
	}
}

void Update(float elapsed, std::vector<Entity*>& entities, Entity& bullet, Entity& explosion, Entity& player){
	static float timer = 0;
	static bool right = 1;
	static float explosion_life = 0;

	timer += elapsed;
	explosion_life += elapsed;
	
	player.Update(elapsed, friction_x, gravity);
	if (player.x >= (6.22 - player.width / 2)){
		player.x = (6.22 - player.width / 2);
	}
	else if ((player.x <= (-6.22 + player.width / 2))){
		player.x = (-6.22 + player.width / 2);
	}
	bullet.Update(elapsed, friction_x, gravity);
	if (
		3.5 <= bullet.y ||
		bullet.y <= -3.5
		){
		bullet.velocity_y = 0;
		bullet.x = 7.0f;
		bullet.y = 0.0f;
	}
	if (explosion_life > 0.2){
		if (explosion.isDead){
			explosion.x = 7.0f;
			explosion.y = 0.0f;
		}
		explosion_life = 0;
	}

	for (size_t i = 0; i < entities.size(); ++i){
		if (entities[i]->hasSqCollision(bullet) && !entities[i]->isDead){
			entities[i]->isDead = 1;
			bullet.velocity_y = 0;
			bullet.x = 7.0f;
			bullet.y = 0.0f;
			explosion.x = entities[i]->x;
			explosion.y = entities[i]->y;
			explosion.isDead = 1;
		}
	}

	if (timer > 1.5f){
	
		if (right){
			for (size_t i = 0; i < entities.size(); ++i){
				if (
					!entities[i]->isDead &&
					(entities[i]->x + entities[i]->width / 2) >= (6.22 - entities[i]->width / 2)
					)
				{
					for (size_t j = 0; j < entities.size(); ++j){
						entities[j]->y -= entities[i]->height;
					}
					right = 0;
					timer = 0.0f;
					return;
				}
			}
			for (size_t i = 0; i < entities.size(); ++i){
				entities[i]->x += entities[i]->width / 2;
			}
		
		}
		else{
			for (size_t i = 0; i < entities.size(); ++i){
				if (!entities[i]->isDead && (entities[i]->x - entities[i]->width / 2) <= (-6.22 + entities[i]->width / 2)){
					for (size_t j = 0; j < entities.size(); ++j){
						entities[j]->y -= entities[j]->height;
					}
					right = 1;
					timer = 0.0f;
					return;
				}
			}
			for (size_t i = 0; i < entities.size(); ++i){
				entities[i]->x -= entities[i]->width / 2;
			}
		}
		timer = 0.0f;
	}

}

void Render(ShaderProgram* program, std::vector<Entity*>& entities, Entity& bullet, Entity& explosion, Entity& player){

	for (size_t i = 0; i < entities.size(); ++i){
		if (!entities[i]->isDead){
			entities[i]->Draw(program);
		}
	}

	player.Draw(program);
	bullet.Draw(program);
	explosion.Draw(program);
	SDL_GL_SwapWindow(displayWindow);
}

