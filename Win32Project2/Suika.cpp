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

	range = false;
	hp = 100.f;

	hpBar = RectangleShape(Vector2f(32, 5));
	hpBar.setPosition(p - Vector2f(0, 5));
	hpBar.setFillColor(Color::Red);
}

void Suika::setMoveAnimation(const Texture &t, float speed)
{
	left = Animation(t, 0, 80, 8, 32, 40, speed);
	right = Animation(t, 0, 120, 8, 32, 40, speed);
	up = Animation(t, 0, 0, 8, 32, 40, speed);
	down = Animation(t, 0, 40, 8, 32, 40, speed);
}

void Suika::setAttackAnimation(const Texture &t, float speed)
{
	leftAttack = Animation(t, 0, 80, 8, 32, 40, speed);
	rightAttack = Animation(t, 0, 120, 8, 32, 40, speed);
	upAttack = Animation(t, 0, 0, 8, 32, 40, speed);
	downAttack = Animation(t, 0, 40, 8, 32, 40, speed);
}