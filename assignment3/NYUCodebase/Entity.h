#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "SheetSprite.h"

struct Entity {

	Entity(GLuint oldTextureID, float res, float u, float v, float width, float height, float size);
	void Draw(ShaderProgram* program);
	void Update(float elapsed, float friction_x, float gravity);
	bool hasSqCollision(Entity other);

	SheetSprite sprite;

	float x;
	float y;
	float rotation;

	float width;
	float height;
	float scale;

	float acceleration_x;

	float velocity_x;
	float velocity_y;

	bool isDead;
	bool isStatic;
};