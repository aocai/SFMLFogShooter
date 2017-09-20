#include "Sakuya.h"

Sakuya::Sakuya(Vector2f p) : Enemy(std::forward<Vector2f>(p))
{
	range = true;
}

void Sakuya::shoot(Vector2f p)
{
	std::unique_ptr<Projectile> cproj = std::make_unique<StraightProjectile>(character.getPosition() + Vector2f(16, 20), p, Vector2f(32,20));
	Vector2f v = p - (character.getPosition() + Vector2f(16, 20));
	Vector2f w(0, -1);
	float angle = atan2(determinant(v, w), dotProduct(v, w)) * 180.0 / 3.14159265;
	cproj->setAnimation(rangedAnimation, -angle);
	enemyProjectile.emplace_back(std::move(cproj));
}

void Sakuya::setMoveAnimation(const Texture &t, float speed)
{
	animationVector[0] = Animation(t, 128, 40, 6, 32, 40, speed);
	animationVector[1] = Animation(t, 64, 80, 6, 32, 40, speed);
	animationVector[2] = Animation(t, 0, 0, 6, 32, 40, speed);
	animationVector[3] = Animation(t, 192, 0, 6, 32, 40, speed);
}

void Sakuya::setAttackAnimation(const Texture &t, float speed)
{
	animationVector[4] = Animation(t, 192, 120, 3, 32, 40, speed);
	animationVector[5] = Animation(t, 32, 160, 3, 32, 40, speed);
	animationVector[6] = Animation(t, 0, 120, 3, 32, 40, speed);
	animationVector[7] = Animation(t, 96, 120, 3, 32, 40, speed);
}

void Sakuya::setRangeAnimation(const Texture &t, float speed)
{
	rangedAnimation = Animation(t, 128, 160, 1, 32, 40, speed);
}


void Sakuya::updateAnimation(const Player &player)
{
	animationVector[currentAni].update();
	if (currentAni >= 4 && animationVector[currentAni].isOver())
	{
		shoot(player.getPosition() + player.getSize() / 2.f);
		setCurrentAnimation(currentAni - 4);
	}
}