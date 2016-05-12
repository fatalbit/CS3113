#ifndef _SHEET_SPRITE
	#define _SHEET_SPRITE
#include "ShaderProgram.h"
#include <map>
#include <string>
#include <vector>
struct SheetSprite{

	SheetSprite();
	SheetSprite::SheetSprite(unsigned int textureID, float x_res, float y_res, int sheet_x, int sheet_y, float padding, int index, float width, float height, float size);

	void Draw(ShaderProgram* program);
	
	float x_res;
	float y_res;
	float size;
	float textureID;
	float u;
	float v;
	float width;
	float height;
	float org_width;
	float org_height;
	int sheet_x;
	int sheet_y;
	float padding;

};
#endif