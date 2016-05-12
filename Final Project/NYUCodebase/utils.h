#include "Entity.h"
#include <vector>

GLuint LoadTexture(const char *image_path, DWORD imageType);
void clear_penetration(Entity& first, std::vector<int> solid, std::vector<int> enemy_markers, unsigned short** level);
void read_level(unsigned short**& level, unsigned& LEVEL_X, unsigned& LEVEL_Y, std::vector<Entity*>& entities, Entity& player , Entity& flag, GLuint sprites, std::string fn);
void render_level(ShaderProgram* program, unsigned LEVEL_X, unsigned LEVEL_Y, unsigned short** level, GLuint level_sprites);
void Draw_Text(ShaderProgram* program, GLuint textureID, float x, float y, float size, float spacing, std::string text);
void Reset_Level(GLuint sprites, Entity& player, std::vector<Entity*>& entities, Entity& flag, unsigned& LEVEL_X, unsigned& LEVEL_Y, unsigned short **& level, std::string fn);