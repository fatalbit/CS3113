#ifndef _ENTITY
	#define _ENTITY
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "SheetSprite.h"

enum EntityType{
	ENTITY_PLAYER,
	ENTITY_ENEMY,
	ENTITY_PROJECTILE,
	ENTITY_POWERUP
};

struct Entity {

	Entity(GLuint oldTextureID, float x_res, float y_res, int sheet_x, int sheet_y, float padding, int index, float width, float height, float size, unsigned health);
	void Draw(ShaderProgram* program);
	void Update(float elapsed, float friction_x, float gravity);
	void en_clear_penetration(Entity& other);
	void set_sprite(int index, float size);
	void set_animation(std::string state);
	void add_animation(std::string state, std::vector<int> indicies);
	bool hasSqCollision(Entity& other);
	void set_collide_false(void);

	SheetSprite sprite;

	float x;
	float y;
	float rotation;

	float width;
	float height;
	float scale;

	float acceleration_x;
	float acceleration_y;

	float velocity_x;
	float velocity_y;

	EntityType entityType;

	int animationCur;

	bool isAlive;
	bool isStatic;
	bool flip;
	bool isflying;
	bool facingRight;

	bool collidedTop;
	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;

	int health;
	int damage;

	std::string animation_state;
	std::map<std::string, std::vector<int>> animations;

};

#endif