#include "Cirno.h"

Cirno::Cirno(Vector2f p) : Enemy(std::forward<Vector2f>(p))
{
}

void Cirno::setMoveAnimation(const Texture &t, float speed)
{
	animationVector[0] = Animation(t, 128, 40, 6, 32, 40, speed);
	animationVector[1] = Animation(t, 64, 80, 6, 32, 40, speed);
	animationVector[2] = Animation(t, 0, 0, 6, 32, 40, speed);
	animationVector[3] = Animation(t, 192, 0, 6, 32, 40, speed);
}

void Cirno::setAttackAnimation(const Texture &t, float speed)
{
	animationVector[4] = Animation(t, 0, 120, 4, 32, 40, speed);
	animationVector[5] = Animation(t, 128, 120, 4, 32, 40, speed);
	animationVector[6] = Animation(t, 0, 0, 6, 32, 40, speed);
	animationVector[7] = Animation(t, 192, 0, 6, 32, 40, speed);
}