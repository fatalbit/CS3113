#include "ShaderProgram.h"
#include <string>

struct DrawSpriteText{
	DrawSpriteText(GLuint textureID);
	void Draw(ShaderProgram* program, std::string text, float size, float spacing);

	GLuint textureID;
	float x;
	float y;
	
};