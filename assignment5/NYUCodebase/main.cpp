/* Eric Liang
	CS3113 Game Programming Assignment 5
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
#include "DrawSpriteText.h"
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

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
#define FREQUENCY 44100
#define CHANNELS 2
#define AUD_BUF_SIZE 4096
#define LEVEL_HEIGHT 12
#define LEVEL_WIDTH 12
#define SPRITE_COUNT_X 30
#define SPRITE_COUNT_Y 30
#define CALC_X(x_index) ((((x_index % SPRITE_COUNT_X))*23.0f)+2)
#define CALC_Y(y_index) ((((y_index % SPRITE_COUNT_Y))*23.0f)+2)
#define TILE_SIZE 0.5f


SDL_Window* displayWindow;
GLuint LoadTexture(const char *image_path, DWORD imageType);
void Setup(SDL_Window** displayWindow, Matrix* projectionMatrix);
void GameProcessEvents(SDL_Event* event, bool* done, float elapsed, std::vector<Entity*> entities, Entity& player);
void GameRender(ShaderProgram* program, std::vector<Entity*>& entities, std::vector<DrawSpriteText> text, unsigned short** level, GLuint level_sprites, Entity& player);
void GameUpdate(float elapsed, std::vector<Entity*>& entities,unsigned short**& level, Entity& player);

void TitleProcessEvents(SDL_Event* event, bool* done);
void TitleRender(ShaderProgram* program, std::vector<DrawSpriteText>& text);

void Reset(std::vector<Entity*> entities, Entity& player);

bool bullet_not_on_screen(Entity bullet);
void move_enemies(std::vector<Entity*>& entities, bool& right, float& timer, size_t startIndex, size_t endIndex);

void clear_penetration(Entity& first, unsigned short** level);
void read_level(unsigned short**& level, std::string fn);
void render_level(ShaderProgram* program, unsigned short** level, GLuint level_sprites);

void update_player_sprite(float elapsed, Entity& player);
float sheet_calc_x(size_t index);
float sheet_calc_y(size_t index);

/*friction and gravity not used just testing*/
const float friction_x = 1.05f;
const float gravity = -9.8;

enum GameState {STATE_TITLE, STATE_GAME};

int state = STATE_GAME;

unsigned int LEVEL_X = 0;
unsigned int LEVEL_Y = 0;

std::vector<int> solid{ 121, 122, 123, 124, 125, 128, 129, 151, 152, 153, 154, 155, 156, 157, 158, 159 };

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

	Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, CHANNELS, AUD_BUF_SIZE);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program.programID);

	GLuint plat_sprites = LoadTexture("spritesheet_rgba.png",GL_RGBA);
	GLuint bg_tex = LoadTexture("backgrounds.png", GL_RGBA);

	Entity bg(bg_tex, 1280.0f, 800.0f, 0.0f, 0.0f, 1280.0f, 800.0f, 7.0f);

	Entity player(plat_sprites, 692.0f, 692.0f, CALC_X(20), CALC_Y(0), 21.0f, 21.0f, 0.5f);
	player.x = 6.0f;
	player.y = -2.0f;

	unsigned short** level = nullptr; 
	
	read_level(level,"mymap.txt");

	while (!done) {
		
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		

		float fixedElapsed = elapsed;
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		switch (state){
		//case(STATE_TITLE) :
		//	TitleProcessEvents(&event,&done);
		//	TitleRender(&program,title_text);
			break;
		case(STATE_GAME) :
			viewMatrix.identity();
			viewMatrix.Translate(-player.x, -player.y, 0);
			
			GameProcessEvents(&event, &done, elapsed, entities, player);
			if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS){
				fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
			}
			while (fixedElapsed >= FIXED_TIMESTEP){
				fixedElapsed -= FIXED_TIMESTEP;
				GameUpdate(FIXED_TIMESTEP, entities,level, player);
			}
			GameUpdate(fixedElapsed, entities, level, player);
			GameRender(&program, entities, text, level, plat_sprites, player);
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
	
	*displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(*displayWindow);
	SDL_GL_MakeCurrent(*displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 720);
	projectionMatrix->setOrthoProjection(-4.43f, 4.43f, -3.5f, 3.5f, -1.0f, 1.0f);
}

void GameProcessEvents(SDL_Event* event, bool* done, float elapsed, std::vector<Entity*> entities, Entity& player){
	while (SDL_PollEvent(event)) {
		//Event Loop
		if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
			*done = true;
		}
		else if (event->type == SDL_KEYDOWN){
			if (event->key.keysym.scancode == SDL_SCANCODE_D){
				player.flip = false;
				player.acceleration_x = 3;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_A){
				player.flip = true;
				player.acceleration_x = -3;
			}
			else if (player.collidedBottom && event->key.keysym.scancode == SDL_SCANCODE_SPACE){
				player.velocity_y = 5.5;
				player.collidedBottom = false;
			}
		}
		else if (event->type == SDL_KEYUP){
			if (event->key.keysym.scancode == SDL_SCANCODE_D){
				player.acceleration_x = 0;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_A){
				player.acceleration_x = 0;
			}
		}
	}
}

void GameUpdate(float elapsed, std::vector<Entity*>& entities, unsigned short**& level, Entity& player){
	player.Update(elapsed, friction_x, gravity);
	update_player_sprite(elapsed, player);
	clear_penetration(player, level);
	
}

void GameRender(ShaderProgram* program, std::vector<Entity*>& entities, std::vector<DrawSpriteText> text, unsigned short** level, GLuint level_sprites, Entity& player){
	glClear(GL_COLOR_BUFFER_BIT);
	for (size_t i = 0; i < entities.size(); ++i){
		entities[i]->Draw(program);
	}
	render_level(program, level,level_sprites);
	player.Draw(program);
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

void clear_penetration(Entity& first, unsigned short** level){
	float penetration = 0;
	int gridX = first.x / TILE_SIZE;
	int gridY = abs(first.y / TILE_SIZE);
	int tile_bottom = abs(first.y - first.height / 2) / TILE_SIZE;
	int tile_top = abs(first.y + first.height / 2) / TILE_SIZE;
	int tile_left = abs(first.x - first.width / 2) / TILE_SIZE;
	int tile_right = abs(first.x + first.width / 2) / TILE_SIZE;
	first.collidedBottom = false;
	first.collidedTop = false;
	first.collidedRight = false;
	first.collidedLeft = false;

	if (std::find(solid.begin(), solid.end(), level[gridY][tile_left]) != solid.end()){
		penetration = abs(((tile_left*TILE_SIZE) + TILE_SIZE) - (first.x - first.width / 2));
		first.collidedLeft = true;
		first.acceleration_y = 0;
		first.velocity_x = 0;
		first.x += penetration;
	}
	else if (std::find(solid.begin(), solid.end(), level[gridY][tile_right]) != solid.end()){
		penetration = abs((tile_right*TILE_SIZE) - (first.x + first.width / 2));
		first.collidedRight = true;
		first.acceleration_y = 0;
		first.velocity_x = 0;
		first.x -= penetration;
	}

	if (std::find(solid.begin(),solid.end(),level[tile_bottom][gridX]) != solid.end()){
		first.acceleration_y = 0;
		penetration = abs((first.y - first.height/2) - (tile_bottom * -TILE_SIZE)) ;
		first.collidedBottom = true;
		first.y += penetration;
	}
	else if (std::find(solid.begin(), solid.end(), level[tile_top][gridX]) != solid.end()){
		penetration = abs(((tile_top * -TILE_SIZE) - TILE_SIZE) - (first.y + first.height / 2));
		first.velocity_y = 0; 
		first.collidedTop = true;
		first.y -= penetration;
	}


}

void update_player_sprite(float elapsed, Entity& player){
	static int index = 28;
	static float timer = 0;
	timer += elapsed;

	if (player.collidedBottom && abs(player.velocity_x) > 1.3f){
		player.set_sprite(CALC_X(index), CALC_Y(0), 21.0f, 21.0f, 0.5f);
		if (timer > 0.2f){
			index = 28 + (++index % 2);
			timer = 0;
		}
	}
	else if (player.collidedBottom && abs(player.velocity_x) < 1.3f){
		player.set_sprite(CALC_X(20), CALC_Y(0), 21.0f, 21.0f, 0.5f);
	}
	else if (!player.collidedBottom){
		player.set_sprite(CALC_X(27), CALC_Y(0), 21.0f, 21.0f, 0.5f);
	}
}

void read_level(unsigned short**& level, std::string fn){
	std::ifstream file(fn);
	std::string line;
	while (std::getline(file,line)){
		if (line == "[header]"){
			while (!line.empty()){
				std::getline(file, line);
				if (!line.compare(0,6,"width=")){
					LEVEL_X = atoi(line.substr(6).c_str());
				}
				else if (!line.compare(0, 7, "height=")){
					LEVEL_Y = atoi(line.substr(7).c_str());
				}
			}
			level = new unsigned short*[LEVEL_Y];
			for (size_t i = 0; i < LEVEL_Y; ++i){
				level[i] = new unsigned short[LEVEL_X];
			}
		}
		else if (line == "[layer]"){
			while (!line.empty()){
				std::getline(file, line);
				if (!line.compare(0, 5, "data=")){
					for (size_t y = 0; y < LEVEL_Y; ++y){
						if (!getline(file, line)) exit(1);
						std::stringstream tok(line);
						std::string tokstring;
						for (size_t x = 0; x < LEVEL_X; ++x){
							getline(tok, tokstring, ',');
							level[y][x] = atoi(tokstring.c_str())-1;
						}
					}
				}
			}
		}
	}
	
	file.close();
}

void render_level(ShaderProgram* program, unsigned short** level, GLuint level_sprites){
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	float u;
	float v;
	unsigned count = 0;

	Matrix levelMatrix;
	levelMatrix.identity();
	program->setModelMatrix(levelMatrix);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, level_sprites);
	for (size_t i = 0; i < LEVEL_Y; ++i){
		for (size_t j = 0; j < LEVEL_X; ++j){

			if (level[i][j] != USHRT_MAX){
				count++;
				u = ((float)(((int)level[i][j]) % SPRITE_COUNT_X) * 23 + 2) / 692.0f; /*exact coords for sprite sheet*/
				v = (floor((float)(((int)level[i][j]) / SPRITE_COUNT_X)) * 23 + 2) / 692.0f; /*each space tile is 21px with 2px for spacing, resolution is 692px*/

				float spriteWidth = 21.0f / 692.0f;
				float spriteHeight = 21.0f / 692.0f;

				vertexData.insert(vertexData.end(),
				{
					TILE_SIZE * j, -TILE_SIZE * i,
					TILE_SIZE * j, (-TILE_SIZE * i) - TILE_SIZE,
					(TILE_SIZE * j) + TILE_SIZE, (-TILE_SIZE*i) - TILE_SIZE,

					TILE_SIZE * j, -TILE_SIZE * i,
					(TILE_SIZE * j) + TILE_SIZE, (-TILE_SIZE*i) - TILE_SIZE,
					(TILE_SIZE*j) + TILE_SIZE, -TILE_SIZE * i
				});
				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, count * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisable(GL_BLEND);
	
}
