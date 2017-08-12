#include "Suika.h"

Suika::Suika(Vector2f p)
{
	character = RectangleShape(Vector2f(32, 40));
	character.setPosition(p);
	velocity = Vector2f(0, 0);
	counter = 4;
	moveState = 0;

	currentAnimation = &down;
	currentAni = 3;
}

Suika::~Suika()
{
}

std::shared_ptr<Projectile> Suika::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj(new CircleProjectile(character.getPosition() + character.getSize() / 2.f, playerPosition));
	return cproj;
}

void Suika::setMoveAnimation(Texture &t, float speed)
{
	left = Animation(t, 0, 80, 8, 32, 40, speed);
	right = Animation(t, 0, 120, 8, 32, 40, speed);
	up = Animation(t, 0, 0, 8, 32, 40, speed);
	down = Animation(t, 0, 40, 8, 32, 40, speed);
}

void Suika::setAttackAnimation(Texture &t, float speed)
{
	leftAttack = Animation(t, 0, 80, 8, 32, 40, speed);
	rightAttack = Animation(t, 0, 120, 8, 32, 40, speed);
	upAttack = Animation(t, 0, 0, 8, 32, 40, speed);
	downAttack = Animation(t, 0, 40, 8, 32, 40, speed);
}