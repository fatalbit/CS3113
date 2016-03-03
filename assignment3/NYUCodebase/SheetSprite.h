#include "ShaderProgram.h"
struct SheetSprite{
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);

	void Draw(ShaderProgram* program);
	
	float size;
	float textureID;
	float u;
	float v;
	float width;
	float height;
};