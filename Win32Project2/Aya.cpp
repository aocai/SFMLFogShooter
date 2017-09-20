#include "Aya.h"

Aya::Aya(Vector2f p) : Enemy(std::forward<Vector2f>(p))
{
}

void Aya::setMoveAnimation(const Texture &t, float speed)
{
	animationVector[0] = Animation(t, 0, 40, 4, 32, 40, speed);
	animationVector[1] = Animation(t, 128, 40, 4, 32, 40, speed);
	animationVector[2] = Animation(t, 0, 0, 4, 32, 40, speed);
	animationVector[3] = Animation(t, 128, 0, 4, 32, 40, speed);
}

void Aya::setAttackAnimation(const Texture &t, float speed)
{
	animationVector[4] = Animation(t, 128, 120, 6, 32, 40, speed);
	animationVector[5] = Animation(t, 64, 160, 6, 32, 40, speed);
	animationVector[6] = Animation(t, 0, 80, 6, 32, 40, speed);
	animationVector[7] = Animation(t, 192, 80, 6, 32, 40, speed);
}