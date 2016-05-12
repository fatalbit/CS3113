#include "Game.h"
#include "config.h"
#include "utils.h"
#include <cstdlib>
void GameProcessEvents(SDL_Event* event, bool* done, float elapsed, std::vector<Entity*> entities, Entity& missle, Entity& player){
	while (SDL_PollEvent(event)) {
		//Event Loop
		if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
			*done = true;
		}
		else if (event->type == SDL_KEYDOWN){
			if (event->key.keysym.scancode == SDL_SCANCODE_D){
				player.acceleration_x = 3;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_A){
				player.acceleration_x = -3;
			}
			else if (player.collidedBottom && event->key.keysym.scancode == SDL_SCANCODE_SPACE){
				player.velocity_y = 5.5;
				player.collidedBottom = false;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_K){
				if (missle.x > player.x + ORTHO_X ||
					missle.x < player.x - ORTHO_X || 
					missle.isAlive == false
					)
				{
					missle.isAlive = true;
					missle.flip = !player.flip;
					missle.y = player.y;
					if (missle.flip){
						missle.x = player.x + player.width;
						missle.acceleration_x = 7.0f;
					}
					else{
						missle.x = player.x - player.width;
						missle.acceleration_x = -7.0f;
					}
				}
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

void GameUpdate(float elapsed, GLuint sprites, std::vector<Entity*>& entities, std::vector<int> solid, std::vector<int> enemy_markers, Entity& flag, Entity& missle, unsigned& LEVEL_X, unsigned& LEVEL_Y, unsigned short**& level, int& points, Entity& player, Entity& powerup, int& state, int& cur_level){
	static float animationTime = 0;

	player.Update(elapsed, FRICTION_X, GRAVITY);
	missle.Update(elapsed, FRICTION_X, GRAVITY);
	flag.Update(elapsed, FRICTION_X, GRAVITY);
	clear_penetration(flag, solid, enemy_markers, level);
	if (powerup.isAlive){
		powerup.Update(elapsed, FRICTION_X, GRAVITY);
		clear_penetration(powerup, solid, enemy_markers, level);
	}
	if (player.hasSqCollision(flag) && player.isAlive){
		if (cur_level < 2){
			cur_level++;
			switch (cur_level){
			case 0:
				Reset_Level(sprites, player, entities, flag, LEVEL_X, LEVEL_Y, level, "level1.txt");
				break;
			case 1:
				Reset_Level(sprites, player, entities, flag, LEVEL_X, LEVEL_Y, level, "level2.txt");
				break;
			case 2:
				Reset_Level(sprites, player, entities, flag, LEVEL_X, LEVEL_Y, level, "level3.txt");
				break;
			default:
				break;
			}
		}
		else{
			state = STATE_END;
		}
	}
	if (powerup.isAlive && player.hasSqCollision(powerup)){
		player.damage++;
		powerup.isAlive = false;
	}
	player.set_collide_false();
	missle.set_collide_false();
	if (missle.x > player.x + ORTHO_X ||
		missle.x < player.x - ORTHO_X){
		missle.isAlive = false;
	}
	for (size_t i = 0; i < entities.size(); ++i){
		entities[i]->set_collide_false();

		if (entities[i]->isAlive){
			entities[i]->Update(elapsed, FRICTION_X, GRAVITY);
			if (entities[i]->hasSqCollision(player)){
				player.health--;
				if (!player.health){
					state = STATE_OVER;
				}
			}
			if (entities[i]->hasSqCollision(missle) && missle.isAlive){
				entities[i]->health -= player.damage;
				if (entities[i]->health <= 0){
					points += 10;
					int drop_rate = rand() % 100;
					switch (player.damage){
					case 1:
						if (drop_rate <= 25){
							powerup.isAlive = true;
							powerup.x = entities[i]->x;
							powerup.y = entities[i]->y;
						}
					default:
						break;
					}
					entities[i]->isAlive = false;
				}
				missle.isAlive = false;
			}
		}

		clear_penetration(*entities[i], solid, enemy_markers, level);

		if (entities[i]->collidedRight){
			entities[i]->acceleration_x = -3.0f;
		}
		else if (entities[i]->collidedLeft){
			entities[i]->acceleration_x = 3.0f;
		}
	}
	clear_penetration(missle, solid, enemy_markers, level);
	if (missle.collidedLeft || missle.collidedRight){ missle.isAlive = false; }
	clear_penetration(player, solid, enemy_markers, level);

}

void GameRender(ShaderProgram* program, SDL_Window*	displayWindow, std::vector<Entity*> entities, Entity& flag, std::string text, Entity missle, unsigned LEVEL_X, unsigned LEVEL_Y, unsigned short** level, GLuint text_sprites, GLuint level_sprites, Entity& player, Entity& powerup){
	glClear(GL_COLOR_BUFFER_BIT);
	render_level(program, LEVEL_X, LEVEL_Y, level, level_sprites);
	for (size_t i = 0; i < entities.size(); ++i){
#if (DEBUG == 1)
		Draw_Text(program, text_sprites, entities[i]->x, entities[i]->y + 0.25f, 0.125f, 0.0f, "TBLR: " + std::to_string(entities[i]->collidedTop) + std::to_string(entities[i]->collidedBottom) + std::to_string(entities[i]->collidedLeft) + std::to_string(entities[i]->collidedRight));
		Draw_Text(program, text_sprites, entities[i]->x, entities[i]->y + 0.5f, 0.125f, 0.0f, "HP " + std::to_string(entities[i]->health));
#endif
		if (entities[i]->isAlive){
			entities[i]->Draw(program);
		}
	}
	if (powerup.isAlive){
		powerup.Draw(program);
	}
	flag.Draw(program);
	player.Draw(program);
	if (missle.isAlive){
		missle.Draw(program);
	}
	Draw_Text(program, text_sprites, player.x - (ORTHO_X - 0.5f), player.y + (ORTHO_Y - 0.25f), 0.25f, 0.0f, text);
#if (DEBUG == 1)
	Draw_Text(program, text_sprites, player.x - (ORTHO_X -0.5f), player.y + (ORTHO_Y - 0.5f), 0.25f, 0.0f, "x: " + std::to_string(player.x) + " y:" + std::to_string(player.y));
	Draw_Text(program, text_sprites, player.x - (ORTHO_X - 0.5f), player.y + (ORTHO_Y - 0.75f), 0.25f, 0.0f, "accel: " + std::to_string(player.acceleration_y));
	Draw_Text(program, text_sprites, player.x - (ORTHO_X - 0.5f), player.y + (ORTHO_Y - 1.0f), 0.25f, 0.0f,  "vel_x: " + std::to_string(player.velocity_x) + " vel_y: " + std::to_string(player.velocity_y));
	Draw_Text(program, text_sprites, player.x - (ORTHO_X - 0.5f), player.y + (ORTHO_Y - 1.25f), 0.25f, 0.0f, "TBLR: " + std::to_string(player.collidedTop) + std::to_string(player.collidedBottom) + std::to_string(player.collidedLeft) + std::to_string(player.collidedRight));
	Draw_Text(program, text_sprites, player.x - (ORTHO_X - 0.5f), player.y + (ORTHO_Y - 1.5f), 0.25f, 0.0f, "mis_x: " + std::to_string(missle.x) + " mis_y:" + std::to_string(missle.y));
	Draw_Text(program, text_sprites, player.x - (ORTHO_X - 0.5f), player.y + (ORTHO_Y - 1.75f), 0.25f, 0.0f, "dmg: " + std::to_string(player.damage));
#endif
	SDL_GL_SwapWindow(displayWindow);
}
