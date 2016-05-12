#ifndef _DRAW_SPRITE_TEXT
#define _DRAW_SPRITE_TEXT
#include "ShaderProgram.h"
#include <string>

struct DrawSpriteText{
	DrawSpriteText(GLuint textureID, std::string text, float size, float spacing);
	void Draw(ShaderProgram* program);

	GLuint textureID;
	std::string text;
	float size;
	float spacing;
	float x;
	float y;
};

#endif