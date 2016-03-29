#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "SheetSprite.h"

enum EntityType{
	ENTITY_PLAYER,
	ENTITY_ENEMY
};

struct Entity {

	Entity(GLuint oldTextureID, float x_res, float y_res, float u, float v, float width, float height, float size);
	void Draw(ShaderProgram* program);
	void Update(float elapsed, float friction_x, float gravity);
	bool hasSqCollision(Entity& other);
	void set_sprite(float u, float v, float width, float height, float size);

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

	bool isStatic;
	bool flip;
	bool collidedTop;
	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;

};