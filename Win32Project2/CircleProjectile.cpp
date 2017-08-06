#include "CircleProjectile.h"
#include "main.h"

CircleProjectile::CircleProjectile()
{
}

CircleProjectile::~CircleProjectile()
{
}

void CircleProjectile::spawn(Vector2f p, Vector2f p2)
{
	CircleShape circ(5);
	circ.setPosition(p);
	circ.setFillColor(Color::Red);
	ci = circ;
	position = p;
	Vector2f v = p2 - p;
	velocity = Vector2f(5.0f * (v / magnitude(v)));
}

Shape* CircleProjectile::getProjectile()
{
	return &ci;
}