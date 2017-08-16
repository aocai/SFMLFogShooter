#pragma once
#include "Enemy.h"
#include "Projectile.h"
#include "CircleProjectile.h"

class Sakuya :
	public Enemy
{
	Animation rangedAnimation;
public:
	std::shared_ptr<Projectile> shoot(Vector2f);
	Sakuya(Vector2f);
	~Sakuya();
	void setMoveAnimation(Texture&, float);
	void setAttackAnimation(Texture&, float);
	void setRangeAnimation(Texture&, float);
	void updateAnimation();
};