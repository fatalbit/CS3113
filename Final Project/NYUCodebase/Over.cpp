#include "Over.h"
#include "config.h"
#include "utils.h"

void OverProcessEvents(SDL_Event* event, int& state, bool& done){
	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT || event->type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event->type == SDL_KEYDOWN){
			if (event->key.keysym.scancode == SDL_SCANCODE_RETURN || event->key.keysym.scancode == SDL_SCANCODE_SPACE){
				state = STATE_TITLE;
			}
		}

	}
}

void OverRender(ShaderProgram* program, SDL_Window* displayWindow, GLuint text_sprites){
	glClear(GL_COLOR_BUFFER_BIT);
	Draw_Text(program, text_sprites, -1.25f, 0.0f, 0.25f, 0.0f, "GAME OVER");
	SDL_GL_SwapWindow(displayWindow);
}