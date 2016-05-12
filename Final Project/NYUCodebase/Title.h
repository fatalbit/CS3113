#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>

#include "ShaderProgram.h"
#include "Entity.h"
#include "DrawSpriteText.h"

void TitleProcessEvents(SDL_Event* event, int& state, bool& done);
void TitleRender(ShaderProgram* program, SDL_Window* displayWindow, GLuint text_sprites);