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
#include "DrawSpriteText.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define RAD_CONV (3.1415926 / 180.0)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define ENEMY_BULLETS 10
#define MAX_LIVES 3

SDL_Window* displayWindow;
GLuint LoadTexture(const char *image_path, DWORD imageType);
void Setup(SDL_Window** displayWindow, Matrix* projectionMatrix);
void GameProcessEvents(SDL_Event* event, bool* done, float elapsed, std::vector<Entity*> entities, Entity& player_bullet, Entity& player);
void GameRender(ShaderProgram* program, std::vector<Entity*>& entities, std::vector<Entity*> enemy_bullets, std::vector<DrawSpriteText> text, Entity& player_bullet, Entity& explosion, Entity& player);
void GameUpdate(float elapsed, std::vector<Entity*>& entities, std::vector<Entity*> enemy_bullets, Entity& player_bullet, Entity& explosion, Entity& player);

void TitleProcessEvents(SDL_Event* event, bool* done);
void TitleRender(ShaderProgram* program, std::vector<DrawSpriteText>& text);

void Reset(std::vector<Entity*> entities, Entity& player);

bool bullet_not_on_screen(Entity bullet);
void move_enemies(std::vector<Entity*>& entities, bool& right, float& timer, size_t startIndex, size_t endIndex);

std::vector<std::pair<float, float>> points;

int lives = MAX_LIVES;

/*friction and gravity not used just testing*/
const float friction_x = 0.1f;
const float gravity = 0.0f;

enum GameState {STATE_TITLE, STATE_GAME};

int state = STATE_TITLE;
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
	std::vector<DrawSpriteText> text;
	std::vector<Entity*> enemy_bullets;
	std::vector<DrawSpriteText> title_text;

	GLuint space_sheet = LoadTexture("sheet.png", GL_RGBA);
	GLuint fontsheet = LoadTexture("font1.png", GL_RGBA);
	GLuint bg = LoadTexture("darkPurple.png", GL_RGB);

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
			
			newEnt->x = (-6.22 + newEnt->width/2 * 11) + (x * (newEnt->width + 0.15));
			newEnt->y = ((3.5 - newEnt->height * 2) - newEnt->height/2 ) - (y * (newEnt->height + 0.2));
			entities.push_back(newEnt);
		}
	}

	for (size_t i = 0; i < entities.size(); ++i){
		points.push_back(std::make_pair(entities[i]->x, entities[i]->y));
	}
	
	Entity player(space_sheet, 1024.0f, 211.0f, 941.0f, 99.0f, 75.0f, 0.5);
	player.y = -3.5 + 0.1 + player.height/2;

	Entity player_bullet(space_sheet, 1024.0f, 843.0f, 789.0f, 13.0f, 57.0f, 0.5);
	player_bullet.x = 7.0;
	
	for (size_t i = 0; i < ENEMY_BULLETS; ++i){
		newEnt = new Entity(space_sheet, 1024.0f, 843.0f, 789.0f, 13.0f, 57.0f, 0.5);
		newEnt->x = 7.0;
		enemy_bullets.push_back(newEnt);
	}

	Entity explosion(space_sheet, 1024.0f, 737.0f, 613.0f, 37.0f, 37.0f, 0.5);
	explosion.x = 7.0;

	DrawSpriteText Life(fontsheet, "LIFE", 0.5, 0);
	Life.y = 3.0f;
	Life.x = 1.0f;
	text.push_back(Life);

	for (size_t i = 0; i < MAX_LIVES; ++i){
		newEnt = new Entity(space_sheet, 1024.0f, 211.0f, 941.0f, 99.0f, 75.0f, 0.5);
		newEnt->y = 3.0f;
		newEnt->x = 1.2f + newEnt->width*3 + ((newEnt->width + 0.1)*i);
		entities.push_back(newEnt);
	}

	DrawSpriteText Title(fontsheet, "Press Enter to Start", 0.5, 0);
	Title.x = -4.7;
	title_text.push_back(Title);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glUseProgram(program.programID);
	
	while (!done) {
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		

		float fixedElapsed = elapsed;
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		switch (state){
		case(STATE_TITLE) :
			TitleProcessEvents(&event,&done);
			TitleRender(&program,title_text);
			break;
		case(STATE_GAME):
			GameProcessEvents(&event, &done, elapsed, entities, player_bullet, player);
			if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS){
				fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
			}
			while (fixedElapsed >= FIXED_TIMESTEP){
				fixedElapsed -= FIXED_TIMESTEP;
				GameUpdate(FIXED_TIMESTEP, entities, enemy_bullets, player_bullet, explosion, player);
			}
			GameUpdate(fixedElapsed, entities, enemy_bullets, player_bullet, explosion, player);
			GameRender(&program, entities, enemy_bullets, text, player_bullet, explosion, player);
			break;
		}
		
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

void GameProcessEvents(SDL_Event* event,bool* done, float elapsed, std::vector<Entity*> entities, Entity& player_bullet, Entity& player){
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
				if (!player.isDead && bullet_not_on_screen(player_bullet))
				{
					player_bullet.x = player.x;
					player_bullet.y = player.y + player_bullet.height;
					player_bullet.velocity_y = 4.0;
				}
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_R){
				Reset(entities,player);
			
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

void GameUpdate(float elapsed, std::vector<Entity*>& entities, std::vector<Entity*> enemy_bullets, Entity& player_bullet, Entity& explosion, Entity& player){
	static float timer = 0;
	static bool right = 1;
	static float explosion_life = 0;
	int randEnemy = 0;
	timer += elapsed;
	explosion_life += elapsed;
	
	size_t count = 44;
	for (size_t i = 0; i < 44; ++i){
		if (entities[i]->isDead){
			--count;
		}
	}

	for (size_t i = 0; i < ENEMY_BULLETS && i < count; ++i){
		if (bullet_not_on_screen(*enemy_bullets[i])){
			randEnemy = rand() % 44;
			
			while (entities[randEnemy]->isDead){
				randEnemy = rand() % 44;
			}
		
			enemy_bullets[i]->x = entities[randEnemy]->x;
			enemy_bullets[i]->y = entities[randEnemy]->y - entities[randEnemy]->height;
			enemy_bullets[i]->velocity_y = -2;
		}
		enemy_bullets[i]->Update(elapsed, friction_x, gravity);
		if (!player.isDead && enemy_bullets[i]->hasSqCollision(player)){
			enemy_bullets[i]->x = 7.0f;
			lives--;
			if (lives <= 0){
				player.isDead = 1;
			}
		}
	}

	

	player.Update(elapsed, friction_x, gravity);
	if (player.x >= (6.22 - player.width / 2)){
		player.x = (6.22 - player.width / 2);
	}
	else if ((player.x <= (-6.22 + player.width / 2))){
		player.x = (-6.22 + player.width / 2);
	}
	player_bullet.Update(elapsed, friction_x, gravity);
	if (bullet_not_on_screen(player_bullet)){
		player_bullet.velocity_y = 0;
		player_bullet.x = 7.0f;
		player_bullet.y = 0.0f;
	}
	if (explosion_life > 0.2){
		if (explosion.isDead){
			explosion.x = 7.0f;
			explosion.y = 0.0f;
		}
		explosion_life = 0;
	}

	for (size_t i = 44; i < 44 + (MAX_LIVES - lives); ++i){ /*show life amount based on lives variable*/
		entities[i]->isDead = 1;
	}

	for (size_t i = 0; i < 44; ++i){
		if (!entities[i]->isDead && entities[i]->hasSqCollision(player_bullet)){
			entities[i]->isDead = 1;
			player_bullet.velocity_y = 0;
			player_bullet.x = 7.0f;
			player_bullet.y = 0.0f;
			explosion.x = entities[i]->x;
			explosion.y = entities[i]->y;
			explosion.isDead = 1;
		}
	}

	move_enemies(entities, right,timer, 0, 44);

}

void GameRender(ShaderProgram* program, std::vector<Entity*>& entities, std::vector<Entity*> enemy_bullets, std::vector<DrawSpriteText> text, Entity& player_bullet, Entity& explosion, Entity& player){
	glClear(GL_COLOR_BUFFER_BIT);
	for (size_t i = 0; i < entities.size(); ++i){
		if (!entities[i]->isDead){
			entities[i]->Draw(program);
		}
	}

	for (size_t i = 0; i < enemy_bullets.size(); ++i){
		enemy_bullets[i]->Draw(program);
	}

	for (size_t i = 0; i < text.size(); ++i){
		text[i].Draw(program);
	}

	if (!player.isDead){
		player.Draw(program);
	}
	player_bullet.Draw(program);
	explosion.Draw(program);


	SDL_GL_SwapWindow(displayWindow);
}

void TitleProcessEvents(SDL_Event* event, bool* done){
	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
			*done = true;
		}
		else if (event->type == SDL_KEYDOWN){
			if (event->key.keysym.scancode == SDL_SCANCODE_RETURN){
				state = STATE_GAME;
			}
		}
		
	}

}

void TitleRender(ShaderProgram* program, std::vector<DrawSpriteText>& text){
	glClear(GL_COLOR_BUFFER_BIT);

	for (size_t i = 0; i < text.size(); ++i){
		text[i].Draw(program);
	}

	SDL_GL_SwapWindow(displayWindow);
}

void move_enemies(std::vector<Entity*>& entities, bool& right, float& timer, size_t startIndex, size_t endIndex){
	if (timer > 1.0f){

		if (right){
			for (size_t i = startIndex; i < endIndex; ++i){
				if (
					!entities[i]->isDead &&
					(entities[i]->x + entities[i]->width / 2) >= (6.22 - entities[i]->width / 2)
					)
				{
					for (size_t j = startIndex; j < endIndex; ++j){
						entities[j]->y -= entities[i]->height;
					}
					right = 0;
					timer = 0.0f;
					return;
				}
			}
			for (size_t i = startIndex; i < endIndex; ++i){
				entities[i]->x += entities[i]->width / 2;
			}

		}
		else{
			for (size_t i = startIndex; i < endIndex; ++i){
				if (!entities[i]->isDead && (entities[i]->x - entities[i]->width / 2) <= (-6.22 + entities[i]->width / 2)){
					for (size_t j = 0; j < 44; ++j){
						entities[j]->y -= entities[j]->height;
					}
					right = 1;
					timer = 0.0f;
					return;
				}
			}
			for (size_t i = startIndex; i < endIndex; ++i){
				entities[i]->x -= entities[i]->width / 2;
			}
		}
		timer = 0.0f;
	}
}

bool bullet_not_on_screen(Entity bullet){
	return 6.22 <= bullet.x || 
		bullet.x <= -6.22 ||
		3.5 <= bullet.y ||
		bullet.y <= -3.5;
}

void Reset(std::vector<Entity*> entities, Entity& player){
	for (size_t i = 0; i < points.size(); ++i){
		entities[i]->x = points[i].first;
		entities[i]->y = points[i].second;
		entities[i]->isDead = 0;
	}

	for (size_t i = entities.size(); i < MAX_LIVES; --i){
		entities[i]->isDead = 0;
	}
	lives = MAX_LIVES;
	player.isDead = 0;

}