#include "Suika.h"

Suika::Suika(Vector2f p) : Enemy(std::forward<Vector2f>(p))
{
}

void Suika::setMoveAnimation(const Texture &t, float speed)
{
	animationVector[0] = Animation(t, 0, 80, 8, 32, 40, speed);
	animationVector[1] = Animation(t, 0, 120, 8, 32, 40, speed);
	animationVector[2] = Animation(t, 0, 0, 8, 32, 40, speed);
	animationVector[3] = Animation(t, 0, 40, 8, 32, 40, speed);
}

void Suika::setAttackAnimation(const Texture &t, float speed)
{
	animationVector[4] = Animation(t, 0, 80, 8, 32, 40, speed);
	animationVector[5] = Animation(t, 0, 120, 8, 32, 40, speed);
	animationVector[6] = Animation(t, 0, 0, 8, 32, 40, speed);
	animationVector[7] = Animation(t, 0, 40, 8, 32, 40, speed);
}