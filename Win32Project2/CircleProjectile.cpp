#include "CircleProjectile.h"
#include "main.h"

CircleProjectile::CircleProjectile()
{
}

CircleProjectile::~CircleProjectile()
{
}

void CircleProjectile::spawn(Vector2f p, Vector2f normal)
{
	CircleShape circ(5);
	circ.setPosition(p);
	ci = circ;
	position = p;
	velocity = Vector2f(5.0f * (normal / magnitude(normal)));
}

void CircleProjectile::destroy()
{

}