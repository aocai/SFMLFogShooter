#pragma once
#include "Enemy.h"
#include "Projectile.h"
#include "CircleProjectile.h"

class Suika :
	public Enemy
{
public:
	std::shared_ptr<Projectile> shoot(Vector2f);
	Suika(Vector2f);
	~Suika();
	void setMoveAnimation(Texture&, float);
	void setAttackAnimation(Texture&, float);
};