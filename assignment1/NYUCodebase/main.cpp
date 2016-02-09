/* Eric Liang
	CS3113 Game Programming Assignment 1
	NetID: ezl207
*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define RAD_CONV (3.1415926 / 180.0)

SDL_Window* displayWindow;
GLuint LoadTexture(const char *image_path, DWORD imageType);
void DrawSprite(GLuint tID, ShaderProgram program);

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	SDL_Event event;
	bool done = false;
	float lastFrameTicks = 0.0f;

	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	
	Matrix projectionMatrix;
	Matrix viewMatrix;

	GLuint green_ship = LoadTexture("playerShip3_green.png",GL_RGBA);
	Matrix green_ship_modelM;

	GLuint yel_ufo = LoadTexture("ufoYellow.png",GL_RGBA);
	Matrix yel_ufo_modelM;

	GLuint dark_purp = LoadTexture("purple.png",GL_RGB);
	Matrix dark_purp_modelM;

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	glUseProgram(program.programID);
	
	float angle = 0;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		angle += elapsed;

		glClear(GL_COLOR_BUFFER_BIT);
		dark_purp_modelM.identity();
		dark_purp_modelM.Scale(7.10, 4, 0);
		program.setModelMatrix(dark_purp_modelM);
		DrawSprite(dark_purp, program);

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		yel_ufo_modelM.identity();
		yel_ufo_modelM.Translate(0, 0.5, 0);
		yel_ufo_modelM.Rotate(angle * 50 * RAD_CONV);	
		program.setModelMatrix(yel_ufo_modelM);
		DrawSprite(yel_ufo, program);

		green_ship_modelM.identity();
		green_ship_modelM.Translate(1.4, -1, 0);
		green_ship_modelM.Rotate(45 * RAD_CONV);
		program.setModelMatrix(green_ship_modelM);
		DrawSprite(green_ship, program);
		
		glDisable(GL_BLEND);
		

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}

GLuint LoadTexture(const char *image_path,DWORD imageType) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, imageType, surface->w, surface->h, 0, imageType,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
	return textureID;
}

void DrawSprite(GLuint tID,ShaderProgram program){
	
	glBindTexture(GL_TEXTURE_2D, tID);

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}