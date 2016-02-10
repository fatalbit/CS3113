class Entity {
	float x_pos;
	float y_pos;
	float angle;

	float speed;
	float x_direction;
	float y_direction;
public:

	Entity();
	bool hasCollision(Entity other);
};