#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include "Entity.h"
#include "ShaderProgram.h"
#include "Matrix.h"
#include "math_internal.h"

#define RAD_CONV (3.1415926 / 180.0)
Entity::Entity(GLuint oldTextureID, float x_res, float y_res, float u, float v, float width, float height, float size) :
	sprite(oldTextureID, x_res, y_res, u, v, width, height, size),
	x(0),
	y(0),
	rotation(0),
	width((width / height)*size),
	height(size),
	acceleration_x(0),
	acceleration_y(0),
	scale(size),
	velocity_x(0),
	velocity_y(0),
	isStatic(false),
	flip(false),
	entityType(ENTITY_ENEMY),
	collidedTop(false),
	collidedBottom(false),
	collidedLeft(false),
	collidedRight(false)
{
	
}

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

void Entity::Draw(ShaderProgram* program){
	Matrix ModelMatrix;
	ModelMatrix.identity();
	ModelMatrix.Translate(x, y, 0);
	ModelMatrix.Rotate(rotation * RAD_CONV);
	if (flip){
		ModelMatrix.Scale(-1, 1, 1);
	}
	
	program->setModelMatrix(ModelMatrix);
	sprite.Draw(program);
}

void Entity::Update( float elapsed, float friction_x, float gravity){
	if (!isStatic){
		if (acceleration_x <= -2.99){
			flip = true;
		}
		else if(acceleration_x >= 2.99){
			flip = false;
		}
		velocity_x = lerp(velocity_x, 0.0f, elapsed * friction_x);
		velocity_y = lerp(velocity_y, 0.0f, elapsed );

		velocity_x += acceleration_x * elapsed;
		velocity_y += gravity * elapsed;

		x += velocity_x * elapsed;
		y += velocity_y * elapsed;
	}
}

void Entity::set_sprite(float u, float v, float width, float height, float size){
	sprite.u = u/sprite.x_res;
	sprite.v = v/sprite.y_res;
	sprite.width = width/sprite.x_res;
	sprite.height = height/sprite.y_res;
	sprite.size = size;
	this->width = width/height*size;
	this->height = size;
	this->scale = size;
}