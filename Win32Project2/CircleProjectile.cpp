#include "CircleProjectile.h"

CircleProjectile::CircleProjectile()
{
}

CircleProjectile::~CircleProjectile()
{
}

CircleProjectile::CircleProjectile(Vector2f p, Vector2f p2)
{
	CircleShape circ(5);
	circ.setPosition(p);
	circ.setFillColor(Color::Red);
	ci = circ;
	position = p;
	Vector2f v = p2 - p;
	velocity = Vector2f(5.0f * (v / magnitude(v)));
}

void CircleProjectile::setAnimation(Texture &t, float speed)
{
	animation = Animation(t, 128, 280, 4, 32, 48, speed);
}

Shape* CircleProjectile::getProjectile()
{
	return &ci;
}

bool CircleProjectile::updateProjectile()
{
	position += velocity;
	//check for bounds
	if (position.x < 0 || position.x > 1280 || position.y < 0 || position.y > 720)
	{
		return false;
	}
	ci.move(velocity);
	return true;
}