#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include "Entity.h"
#include "ShaderProgram.h"

Entity::Entity(GLuint oldTextureID){
	this->textureID = oldTextureID;

	this->x = 0.0f;
	this->y = 0.0f;
	this->rotation = 0.0f;

	this->width = 1.0f;
	this->height = 1.0f;

	this->speed = 0.0f;
	this->direction_x = 0.0f;
	this->direction_y = 0.0f;
}

void Entity::Draw(ShaderProgram* program){


	glBindTexture(GL_TEXTURE_2D, this->textureID);

	float vertices[] = {
		-0.5, -0.5,
		0.5, -0.5,
		0.5, 0.5,
		-0.5, -0.5,
		0.5, 0.5,
		-0.5, 0.5 };

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoords[] = {
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		0.0, 0.0
	};

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	
}