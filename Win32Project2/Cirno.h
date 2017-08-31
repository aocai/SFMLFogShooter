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
	Cirno(Vector2f);
	void setMoveAnimation(const Texture&, float) override;
	void setAttackAnimation(const Texture&, float) override;
};