#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "SheetSprite.h"
#include "ShaderProgram.h"

SheetSprite::SheetSprite(unsigned int textureID, float x_res, float y_res, float u, float v, float width, float height, float size) :
	textureID(textureID),
	x_res(x_res),
	y_res(y_res),
	u(u/x_res),
	v(v/y_res),
	width(width/x_res),
	height(height/y_res),
	size(size)
{
	
}


void SheetSprite::Draw(ShaderProgram* program){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textureID); 

	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};

	float aspect = width / height;

	float vertices[] = {
		-0.5f * size * aspect, -0.5 * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, 0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size
	};

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisable(GL_BLEND);
}