/* Eric Liang
	CS3113 Game Programming Assignment 2
	NetID: ezl207
*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "ShaderProgram.h"
#include "Entity.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define RAD_CONV (3.1415926 / 180.0)

SDL_Window* displayWindow;
GLuint LoadTexture(const char *image_path, DWORD imageType);
void Setup(SDL_Window** displayWindow, Matrix* projectionMatrix);
void ProcessEvents(SDL_Event* event, bool* done, float elapsed, std::vector<Entity>& entities);
void Render(ShaderProgram* program, Matrix* projectionMatrix, Matrix* viewMatrix, std::vector<Matrix>& modelM, std::vector<Entity>& entities,Entity ball, Matrix ballM);
void Update(std::vector<Entity>& entities,Entity& ball,float& ball_angle,float elapsed);
bool hasSqCollision(Entity first, Entity second);
void paddleReverse(float& angle);


int main(int argc, char *argv[])
{
	
	SDL_Event event;
	bool done = false;
	float lastFrameTicks = 0.0f;
	float ball_angle = 0.0f;
	srand(time(NULL));

	Matrix projectionMatrix;
	Matrix viewMatrix;
	Setup(&displayWindow, &projectionMatrix);

	GLuint paddle = LoadTexture("blue_panel.png", GL_RGBA);

	Matrix leftM;
	Entity leftPaddle(paddle);
	leftPaddle.width = 0.3f;
	leftPaddle.x = -3.2f;
	leftPaddle.speed = 2.0f;

	Matrix rightM;
	Entity rightPaddle(paddle);
	rightPaddle.width = 0.3f;
	rightPaddle.x = 3.2f;
	rightPaddle.speed = 2.0f;

	Matrix ballM;
	GLuint ballT = LoadTexture("green_panel.png", GL_RGBA);
	Entity ball(ballT);
	ball.height = 0.25f;
	ball.width = 0.25f;
	ball.speed = 3.0f;
	if (rand() % 1){
		ball_angle = (rand() % 90 + 135);
	}
	else{
		ball_angle = (rand() % 90 + 316);
	}

	std::vector<Entity> entities;
	entities.push_back(leftPaddle);
	entities.push_back(rightPaddle);


	std::vector<Matrix> modelM;
	modelM.push_back(leftM);
	modelM.push_back(rightM);


	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glUseProgram(program.programID);
	
	while (!done) {
		

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		ProcessEvents(&event, &done, elapsed, entities);

		Update(entities,ball,ball_angle,elapsed);
		glClear(GL_COLOR_BUFFER_BIT);

		Render(&program, &projectionMatrix,&viewMatrix, modelM, entities,ball,ballM);
		
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

void Setup(SDL_Window** displayWindow,Matrix* projectionMatrix){
	SDL_Init(SDL_INIT_VIDEO);
	
	*displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(*displayWindow);
	SDL_GL_MakeCurrent(*displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 360);
	projectionMatrix->setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
}

void ProcessEvents(SDL_Event* event,bool* done, float elapsed, std::vector<Entity>& entities){
	while (SDL_PollEvent(event)) {
		//Event Loop
		if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
			*done = true;
		}
		else if (event->type == SDL_KEYDOWN){
			if (event->key.keysym.scancode == SDL_SCANCODE_W){
				entities[0].direction_y = 1;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_S){
				entities[0].direction_y = -1;
			}
			
			if (event->key.keysym.scancode == SDL_SCANCODE_UP){
				entities[1].direction_y = 1;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_DOWN){
				entities[1].direction_y = -1;
			}
		}
		else if (event->type == SDL_KEYUP){
			if (event->key.keysym.scancode == SDL_SCANCODE_W){
				entities[0].direction_y = 0;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_S){
				entities[0].direction_y = 0;
			}

			if (event->key.keysym.scancode == SDL_SCANCODE_UP){
				entities[1].direction_y = 0;
			}
			else if (event->key.keysym.scancode == SDL_SCANCODE_DOWN){
				entities[1].direction_y = 0;
			}
		}

	}
}

void Update(std::vector<Entity>& entities, Entity& ball, float& ball_angle, float elapsed){
	for (size_t i = 0; i < entities.size(); ++i){
		entities[i].x += entities[i].direction_x * elapsed * entities[i].speed;

		entities[i].y += entities[i].direction_y * elapsed * entities[i].speed;
		if (entities[i].y > (2.0 - (entities[i].height) / 2)){
			entities[i].y = (2.0 - (entities[i].height) / 2);
		}
		else if (entities[i].y < (-2.0 + (entities[i].height) / 2)){
			entities[i].y = -2.0 + (entities[i].height)/2;
		}
	}

	if (ball.x > 3.55 || ball.x < -3.55){
		ball.x = 0;
		ball.y = 0;
		if (rand() % 1){
			ball_angle = (rand() % 90 + 135);
		}
		else{
			ball_angle = (rand() % 90 + 315);
		}
	}
	else{
		if (hasSqCollision(entities[0], ball)){
			paddleReverse(ball_angle);
		}
		else if (hasSqCollision(entities[1], ball)){
			paddleReverse(ball_angle);
		}
		if (ball.y >= (2.0 - (ball.height) / 2) || ball.y <= (-2.0 + (ball.height) / 2)){
			ball_angle = (ball_angle - 360) * -1;
		}

		ball.x += cos(ball_angle * RAD_CONV) * elapsed * ball.speed;
		ball.y += sin(ball_angle * RAD_CONV) * elapsed * ball.speed;
	}
	

}

void Render(ShaderProgram* program, Matrix* projectionMatrix, Matrix* viewMatrix, std::vector<Matrix>& modelM, std::vector<Entity>& entities, Entity ball, Matrix ballM){
	program->setProjectionMatrix(*projectionMatrix);
	program->setViewMatrix(*viewMatrix);

	for (size_t i = 0; i < entities.size(); ++i){
		modelM[i].identity();
		modelM[i].Translate(entities[i].x, entities[i].y, 0);
		modelM[i].Scale(entities[i].width, entities[i].height, 1.0f);
		program->setModelMatrix(modelM[i]);
		entities[i].Draw(program);
	}

	ballM.identity();
	ballM.Translate(ball.x, ball.y, 0);
	ballM.Scale(ball.width, ball.height, 1.0f);
	program->setModelMatrix(ballM);
	ball.Draw(program);

	SDL_GL_SwapWindow(displayWindow);
}

bool hasSqCollision(Entity first, Entity second){
	if (
		first.x + (first.width / 2) <= second.x - (second.width/2) || //F.Right < S.Left
		first.x - (first.width / 2) >= second.x + (second.width / 2) || //F.Left >  R.Right
		first.y + (first.height / 2) <= second.y - (second.height/2) ||  //F.Top < S.Bottom
		first.y - (first.height / 2) >= second.y + (second.height/2) //F.Bottom > S.Top
		) {
		return false;
	}
	return true;
}

void paddleReverse(float& angle){
	if (0 < angle < 90 || 180 < angle < 270){
		angle += 90;
	}
	else{
		angle -= 90;
	}
	

}
