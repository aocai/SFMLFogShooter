#pragma once
#include "Enemy.h"
#include "Projectile.h"
#include "CircleProjectile.h"

class Sakuya :
	public Enemy
{
	Animation rangedAnimation;
public:
	void shoot(Vector2f) override;
	Sakuya(Vector2f);
	void setMoveAnimation(const Texture&, float) override;
	void setAttackAnimation(const Texture&, float) override;
	void setRangeAnimation(const Texture&, float) override;
	void updateAnimation(const Player&) override;
};