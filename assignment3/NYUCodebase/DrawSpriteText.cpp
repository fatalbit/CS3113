#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "DrawSpriteText.h"
#include "Matrix.h"
#include <vector>
#include <string>

DrawSpriteText::DrawSpriteText(GLuint textureID, std::string text, float size, float spacing) :
textureID(textureID),
x(0),
y(0),
size(size),
spacing(spacing),
text(text)
{

}

void DrawSpriteText::Draw(ShaderProgram* program){
	Matrix modelMatrix;
	modelMatrix.identity();
	modelMatrix.Translate(x, y, 0);
	program->setModelMatrix(modelMatrix);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textureID);
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	float texture_size = 1.0 / 16.0f;

	for (int i = 0; i < text.size(); ++i){
		float texture_x = (float)(((int)text[i]) % 16) / 16;
		float texture_y = (float)(((int)text[i]) / 16) / 16;
		vertexData.insert(vertexData.end(), {
			((size + spacing)*i) + (-0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (0.5f * size), -0.5f * size,
			((size + spacing)*i) + (0.5f * size), 0.5f * size,
			((size + spacing)*i) + (-0.5f * size), -0.5f * size
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size
		});
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisable(GL_BLEND);
}