#pragma once
#include "Enemy.h"
#include "Projectile.h"
#include "CircleProjectile.h"

class Sakuya :
	public Enemy
{
	Animation rangedAnimation;
public:
	std::shared_ptr<Projectile> shoot(Vector2f) override;
	Sakuya(Vector2f);
	void setMoveAnimation(Texture&, float) override;
	void setAttackAnimation(Texture&, float) override;
	void setRangeAnimation(Texture&, float) override;
	void updateAnimation() override;
};