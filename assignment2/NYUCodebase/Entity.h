#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"

struct Entity {

	Entity(GLuint oldTextureID);
	void Draw(ShaderProgram* program);
	
	GLuint textureID;

	float x;
	float y;
	float rotation;

	float width;
	float height;

	float speed;
	float direction_x;
	float direction_y;

};