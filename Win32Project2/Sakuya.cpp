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

	range = true;
}

std::shared_ptr<Projectile> Sakuya::shoot(Vector2f p)
{
	std::shared_ptr<Projectile> cproj(new CircleProjectile(character.getPosition() + Vector2f(16, 20), p));
	Vector2f v = p - (character.getPosition() + Vector2f(16, 20));
	Vector2f w(0, -1);
	float angle = atan2(determinant(v, w), dotProduct(v, w)) * 180.0 / 3.14159265;
	cproj->setAnimation(rangedAnimation, -angle);
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

void Sakuya::setRangeAnimation(Texture &t, float speed)
{
	rangedAnimation = Animation(t, 128, 160, 1, 32, 40, speed);
}


void Sakuya::updateAnimation()
{
	currentAnimation->update();
	if (currentAni >= 4 && currentAnimation->isOver())
	{
		enemyProjectile.push_back(shoot(target));
		setCurrentAnimation(currentAni - 4);
	}
}