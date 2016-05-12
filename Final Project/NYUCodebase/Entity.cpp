#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include "Entity.h"
#include "ShaderProgram.h"
#include "Matrix.h"
#include "math_internal.h"
#include "config.h"

#define RAD_CONV (3.1415926 / 180.0)
Entity::Entity(GLuint oldTextureID, float x_res, float y_res, int sheet_x, int sheet_y, float padding, int index, float width, float height, float size, unsigned health) :
	sprite(oldTextureID, x_res, y_res, sheet_x, sheet_y, padding, index, width, height, size),
	x(0),
	y(0),
	rotation(0),

	width((width / height) * size),
	height(size),

	acceleration_x(0),
	acceleration_y(0),

	scale(size),

	velocity_x(0),
	velocity_y(0),

	//animationTime(0),
	animationCur(0),

	isAlive(true),
	isStatic(false),
	flip(false),
	isflying(false),
	facingRight(false),

	entityType(ENTITY_ENEMY),

	collidedTop(false),
	collidedBottom(false),
	collidedLeft(false),
	collidedRight(false),

	health(health)
{
	
};

bool Entity::hasSqCollision(Entity& other){
	if (
		this->x + (this->width / 2) <= other.x - (other.width / 2) || //F.Right < S.Left
		this->x - (this->width / 2) >= other.x + (other.width / 2) || //F.Left >  R.Right
		this->y + (this->height / 2) <= other.y - (other.height / 2) ||  //F.Top < S.Bottom
		this->y - (this->height / 2) >= other.y + (other.height / 2) //F.Bottom > S.Top
		) {
		return false;
	}
	return true;
}

void Entity::en_clear_penetration(Entity& other){
	if ((this->x + (this->width / 2)) > other.x - (other.width / 2)){ //this right > other left
		this->collidedRight = true;
		other.collidedLeft = true;
		this->x -= (this->x + this->width / 2) - (other.x - other.width / 2);
	} 
	else if ((this->x - (this->width / 2)) < (other.x + (other.width / 2))) //F.Left <  R.Right
	{
		this->collidedLeft = true;
		other.collidedRight = true;
		this->x += (other.x + other.width / 2) - (this->x - this->width / 2);
	}

	if (this->y + (this->height / 2) > other.y - (other.height / 2)) //F.Top > S.Bottom
	{
		this->collidedTop = true;
		other.collidedBottom = true;
		this->y += (this->y + this->height / 2) - (other.y - other.height / 2);
	} 
	else if (this->y - (this->height / 2) < (other.y + (other.height / 2))) { //F.Bottom < S.Top
		this->collidedBottom = true;
		other.collidedTop = true;
		this->y -= (other.y + other.height / 2) - (this->y - this->height / 2);
	}

}

void Entity::Draw(ShaderProgram* program){
	Matrix ModelMatrix;
	ModelMatrix.identity();
	ModelMatrix.Translate(x, y, 0);
	ModelMatrix.Rotate(rotation * RAD_CONV);
	
	if (flip){
		ModelMatrix.Scale(-1, 1, 1);
	}
	set_sprite(animationCur, sprite.size);
	
	
	program->setModelMatrix(ModelMatrix);
	sprite.Draw(program);

}

void Entity::Update( float elapsed, float friction_x, float gravity){
	static float animationTime;
	if (!isStatic && isAlive){
		if (acceleration_x <= -2.99){
			flip = facingRight;
		}
		else if (acceleration_x >= 2.99){
			flip = !facingRight;
		}

		velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
		velocity_x += acceleration_x * elapsed;
		x += velocity_x * elapsed;
		if (!isflying){
			velocity_y = lerp(velocity_y, 0.0f, elapsed);
			velocity_y += gravity * elapsed;
			y += velocity_y * elapsed;
		}
		if (entityType == ENTITY_ENEMY){
			animation_state = "moving";
		}
		if (entityType == ENTITY_PLAYER){
			if (!collidedBottom){
				animation_state = "jumping";
			}
			else if ((velocity_x > 1.0 || velocity_x < -1.0) && collidedBottom){
				animation_state = "moving";
			}
			else if (-0.7 < velocity_x && velocity_x < 0.7 && collidedBottom){
				animation_state = "idle";
			}
		}
		if (entityType == ENTITY_PROJECTILE){
			animation_state = "moving";
		}
		animationTime = animationTime + elapsed;
		float animationValue = map_value(animationTime, 0.0, ANIMATION_SPEED / 2, 0, animations[animation_state].size() - 1);
		animationCur = animations[animation_state][animationValue];
		if (animationTime > ANIMATION_SPEED){
			animationTime = 0;
		}
	}
}

void Entity::set_sprite(int index, float size){
	int x = ((float)(((int)index) % sprite.sheet_x) * (sprite.org_width + sprite.padding) + sprite.padding);
	int y = (floor((float)(((int)index) / sprite.sheet_y)) * (sprite.org_height + sprite.padding) + sprite.padding);
	sprite.u = x / sprite.x_res;
	sprite.v = y / sprite.y_res;
	this->width = (sprite.org_width / sprite.org_height) * size;
	this->height = size;
	this->scale = size;
}

void Entity::set_animation(std::string state){
	animation_state = state;
}

void Entity::add_animation(std::string state, std::vector<int> indicies){
	animations.insert(std::pair<std::string, std::vector<int>>(state, indicies));
}

void Entity::set_collide_false(void){
	collidedBottom = false;
	collidedTop = false;
	collidedLeft = false;
	collidedRight = false;
}