#include "Aya.h"

Aya::Aya(Vector2f p)
{
	character = RectangleShape(Vector2f(32, 40));
	character.setPosition(p);
	velocity = Vector2f(0, 0);
	counter = 4;
	moveState = 0;

	currentAnimation = &down;
	currentAni = 3;

	range = false;
}

std::shared_ptr<Projectile> Aya::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj(new CircleProjectile(character.getPosition() + character.getSize()/2.f, playerPosition));
	return cproj;
}

void Aya::setMoveAnimation(Texture &t, float speed)
{
	left = Animation(t, 0, 40, 4, 32, 40, speed);
	right = Animation(t, 128, 40, 4, 32, 40, speed);
	up = Animation(t, 0, 0, 4, 32, 40, speed);
	down = Animation(t, 128, 0, 4, 32, 40, speed);
}

void Aya::setAttackAnimation(Texture &t, float speed)
{
	leftAttack = Animation(t, 128, 120, 6, 32, 40, speed);
	rightAttack = Animation(t, 64, 160, 6, 32, 40, speed);
	upAttack = Animation(t, 0, 80, 6, 32, 40, speed);
	downAttack = Animation(t, 192, 80, 6, 32, 40, speed);
}