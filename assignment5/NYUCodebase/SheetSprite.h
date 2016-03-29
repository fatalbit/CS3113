#include "ShaderProgram.h"
struct SheetSprite{
	SheetSprite();
	SheetSprite(unsigned int textureID, float x_res, float y_res, float u, float v, float width, float height, float size);

	void Draw(ShaderProgram* program);
	float x_res;
	float y_res;
	float size;
	float textureID;
	float u;
	float v;
	float width;
	float height;
};