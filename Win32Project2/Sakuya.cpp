#include "Sakuya.h"

Sakuya::Sakuya(Vector2f p)
{
	character = RectangleShape(Vector2f(32, 40));
	character.setPosition(p);
	velocity = Vector2f(0, 0);
	counter = 4;
	moveState = 0;

	currentAnimation = &down;
	currentAni = 3;
}

Sakuya::~Sakuya()
{
}

std::shared_ptr<Projectile> Sakuya::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj(new CircleProjectile(character.getPosition() + character.getSize() / 2.f, playerPosition));
	return cproj;
}

void Sakuya::setMoveAnimation(Texture &t, float speed)
{
	left = Animation(t, 128, 40, 6, 32, 40, speed);
	right = Animation(t, 64, 80, 6, 32, 40, speed);
	up = Animation(t, 0, 0, 6, 32, 40, speed);
	down = Animation(t, 192, 0, 6, 32, 40, speed);
}

void Sakuya::setAttackAnimation(Texture &t, float speed)
{
	leftAttack = Animation(t, 192, 120, 3, 32, 40, speed);
	rightAttack = Animation(t, 32, 160, 3, 32, 40, speed);
	upAttack = Animation(t, 0, 120, 3, 32, 40, speed);
	downAttack = Animation(t, 96, 120, 3, 32, 40, speed);
}