#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

struct Entity {

	void Draw();

	float x_pos;
	float y_pos;
	float angle;

	GLuint textureID;

	float speed;
	float x_direction;
	float y_direction;

};