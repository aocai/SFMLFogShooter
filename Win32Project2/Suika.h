#pragma once
#include "Enemy.h"
#include "Projectile.h"
#include "CircleProjectile.h"

class Suika :
	public Enemy
{
public:
	std::shared_ptr<Projectile> shoot(Vector2f) override;
	Suika(Vector2f);
	void setMoveAnimation(Texture&, float) override;
	void setAttackAnimation(Texture&, float) override;
};