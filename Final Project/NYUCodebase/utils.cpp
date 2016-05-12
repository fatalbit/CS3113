
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "config.h"
#include "utils.h"
#include "DrawSpriteText.h"


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

void clear_penetration(Entity& first, std::vector<int> solid, std::vector<int> enemy_markers, unsigned short** level){
	float penetration = 0;
	int gridX = first.x / TILE_SIZE;
	int gridY = abs(first.y / TILE_SIZE);
	int tile_bottom = fabs(first.y - first.height / 2) / TILE_SIZE;
	int tile_top = fabs(first.y + first.height / 2) / TILE_SIZE;
	int tile_left = fabs(first.x - first.width / 2) / TILE_SIZE;
	int tile_right = fabs(first.x + first.width / 2) / TILE_SIZE;

	if (first.entityType == ENTITY_ENEMY){
		if (std::find(solid.begin(), solid.end(), level[gridY][tile_left]) != solid.end() || std::find(enemy_markers.begin(), enemy_markers.end(), level[gridY][tile_left]) != enemy_markers.end()){ //left
			penetration = fabs(((tile_left*TILE_SIZE) + TILE_SIZE) - (first.x - first.width / 2));
			first.collidedLeft = true;
			first.acceleration_y = 0;
			first.velocity_x = 0;
			first.x += penetration;
		}
		else if (std::find(solid.begin(), solid.end(), level[gridY][tile_right]) != solid.end() || std::find(enemy_markers.begin(), enemy_markers.end(), level[gridY][tile_right]) != enemy_markers.end()){ //right
			penetration = fabs((tile_right*TILE_SIZE) - (first.x + first.width / 2));
			first.collidedRight = true;
			first.acceleration_y = 0;
			first.velocity_x = 0;
			first.x -= penetration;
		}

		if (std::find(solid.begin(), solid.end(), level[tile_bottom][gridX]) != solid.end() || std::find(enemy_markers.begin(), enemy_markers.end(), level[tile_bottom][gridX]) != enemy_markers.end()){ //bottom
			first.acceleration_y = 0;
			penetration = abs((first.y - first.height / 2) - (tile_bottom * -TILE_SIZE));
			first.collidedBottom = true;
			first.y += penetration;
		}

		else if (std::find(solid.begin(), solid.end(), level[tile_top][gridX]) != solid.end() || std::find(enemy_markers.begin(), enemy_markers.end(), level[tile_top][gridX]) != enemy_markers.end()){ //top
			penetration = abs(((tile_top * -TILE_SIZE) - TILE_SIZE) - (first.y + first.height / 2));
			first.velocity_y = 0;
			first.collidedTop = true;
			first.y -= penetration;
		}
	
	}
	else{
		if (std::find(solid.begin(), solid.end(), level[gridY][tile_left]) != solid.end()){ //left
			penetration = fabs(((tile_left*TILE_SIZE) + TILE_SIZE) - (first.x - first.width / 2));
			first.collidedLeft = true;
			first.acceleration_y = 0;
			first.velocity_x = 0;
			first.x += penetration;
		}
		else if (std::find(solid.begin(), solid.end(), level[gridY][tile_right]) != solid.end()){ //right
			penetration = fabs((tile_right*TILE_SIZE) - (first.x + first.width / 2));
			first.collidedRight = true;
			first.acceleration_y = 0;
			first.velocity_x = 0;
			first.x -= penetration;
		}

		if (std::find(solid.begin(), solid.end(), level[tile_bottom][gridX]) != solid.end()){ //bottom
			first.acceleration_y = 0;
			penetration = abs((first.y - first.height / 2) - (tile_bottom * -TILE_SIZE));
			first.collidedBottom = true;
			first.y += penetration;
		}
		else if (std::find(solid.begin(), solid.end(), level[tile_top][gridX]) != solid.end()){ //top
			penetration = abs(((tile_top * -TILE_SIZE) - TILE_SIZE) - (first.y + first.height / 2));
			first.velocity_y = 0;
			first.collidedTop = true;
			first.y -= penetration;
		}
	}
}

void read_level(unsigned short**& level, unsigned& LEVEL_X, unsigned& LEVEL_Y, std::vector<Entity*>& entities, Entity& player, Entity& flag, GLuint sprites, std::string fn){
	std::ifstream file(fn);
	std::string line;
	float x = 0;
	float y = 0;
	Entity* newEnt = nullptr;
	EntType enm_type;

	while (std::getline(file, line)){
		if (line == "[header]"){
			while (!line.empty()){
				if (!std::getline(file, line))break;
				if (!line.compare(0, 6, "width=")){
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
				if (!std::getline(file, line)) break;
				if (!line.compare(0, 5, "data=")){
					for (size_t y = 0; y < LEVEL_Y; ++y){
						if (!getline(file, line)) exit(1);
						std::stringstream tok(line);
						std::string tokstring;
						for (size_t x = 0; x < LEVEL_X; ++x){
							getline(tok, tokstring, ',');
							level[y][x] = atoi(tokstring.c_str()) - 1;
						}
					}
				}
			}
		}
		else if (line == "[Objects]"){
			while (!line.empty()){
				if (!std::getline(file, line)) break;
				if (!line.compare(0, 5, "type=")){
					line = line.substr(5);
					if (line == "bee"){
						enm_type = BEE;
					}
					else if (line == "mouse"){
						enm_type = MOUSE;
					}
					else if (line == "bat"){
						enm_type = BAT;
					}
					else if (line == "player"){
						enm_type = PLAYER;
					}
					else if (line == "flag"){
						enm_type = FLAG;
					}
				}
				else if (!line.compare(0, 9, "location=")){
					line = line.substr(9);
					std::stringstream tok2(line);
					std::string tokstring;
					if (!getline(tok2, tokstring, ',')) break;
					x = (-TILE_SIZE) + atoi(tokstring.c_str());
					if (!getline(tok2, tokstring, ','))break;
					y = (-TILE_SIZE) + atoi(tokstring.c_str());
				}
			}
			switch (enm_type){
			case BEE:
				newEnt = new Entity(sprites, 692.0f, 692.0f, 30,30, 2, 354.0f, 21.0f, 21.0f, 0.5f, 2);
				newEnt->x = x*TILE_SIZE;
				newEnt->y = y*-TILE_SIZE;
				newEnt->isflying = true;
				newEnt->acceleration_x = -3.0f;
				newEnt->add_animation("moving", { 354, 355 });
				newEnt->set_animation("moving");
				entities.push_back(newEnt);
				break;
			case MOUSE:
				newEnt = new Entity(sprites, 692.0f, 692.0f, 30, 30, 2, 384.0f, 21.0f, 21.0f, 0.5f, 1);
				newEnt->x = x*TILE_SIZE;
				newEnt->y = y*-TILE_SIZE;
				newEnt->acceleration_x = 3.0f;
				newEnt->add_animation("moving", { 384, 385 });
				newEnt->set_animation("moving");
				entities.push_back(newEnt);
				break;
			case BAT:
				newEnt = new Entity(sprites, 692.0f, 692.0f, 30, 30, 2, 440.0f, 21.0f, 21.0f, 0.5f, 3);
				newEnt->x = x*TILE_SIZE;
				newEnt->y = y*-TILE_SIZE;
				newEnt->acceleration_x = 3.0f;
				newEnt->isflying = true;
				newEnt->add_animation("moving", { 440, 441, 442});
				newEnt->set_animation("moving");
				entities.push_back(newEnt);
				break;
			case PLAYER:
				player.x = x*TILE_SIZE;
				player.y = y*-TILE_SIZE;
				break;
			case FLAG:
				flag.x = x*TILE_SIZE;
				flag.y = y*-TILE_SIZE;
				break;
			default:
				break;
			}
		}
	}
	file.close();
}

void render_level(ShaderProgram* program, unsigned LEVEL_X, unsigned LEVEL_Y, unsigned short** level, GLuint level_sprites){
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

			if (level[i][j] != USHRT_MAX && level[i][j] != 899){
				count++;
				u = ((float)(((int)level[i][j]) % SPRITE_X) * 23 + 2.0f) / 692.0f; /*exact coords for sprite sheet*/
				v = (floor((float)(((int)level[i][j]) / SPRITE_Y)) * 23 + 2.0f) / 692.0f; /*each space tile is 21px with 2px for spacing, resolution is 692px*/

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

void Draw_Text(ShaderProgram* program, GLuint textureID, float x, float y, float size, float spacing, std::string text){
	DrawSpriteText text_a(textureID, text, size, spacing);
	text_a.x = x;
	text_a.y = y;

	if (!text.empty()){
		text_a.Draw(program);
	}
}

void Reset_Level(GLuint sprites, Entity& player, std::vector<Entity*>& entities, Entity& flag, unsigned& LEVEL_X, unsigned& LEVEL_Y, unsigned short **& level, std::string fn){
	player.health = 1;
	player.damage = 1;
	player.velocity_x = 0;
	player.velocity_y = 0;

	for (size_t i = 0; i < entities.size(); ++i){
		delete entities[i];
	}
	entities.clear();

	for (size_t i = 0; i < LEVEL_Y; ++i){
		delete[] level[i];
	}
	delete[] level;

	read_level(level, LEVEL_X, LEVEL_Y, entities, player, flag, sprites, fn);
}