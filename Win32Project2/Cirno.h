#pragma once
#include "Enemy.h"
#include <stack>
#include "Projectile.h"
#include "CircleProjectile.h"
#include "AStar.h";

class Cirno : 
	public Enemy
{
public:
	std::shared_ptr<Projectile> shoot(Vector2f) override;
	Cirno(Vector2f);
	void setMoveAnimation(Texture&, float) override;
	void setAttackAnimation(Texture&, float) override;
};