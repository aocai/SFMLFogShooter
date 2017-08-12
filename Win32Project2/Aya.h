#pragma once
#include "Enemy.h"
#include <stack>
#include "Projectile.h"
#include "CircleProjectile.h"
#include "AStar.h";

class Aya :
	public Enemy
{
public:
	std::shared_ptr<Projectile> shoot(Vector2f);
	Aya(Vector2f);
	~Aya();
	void setMoveAnimation(Texture&, float);
	void setAttackAnimation(Texture&, float);
};