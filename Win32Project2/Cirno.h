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
	std::shared_ptr<Projectile> shoot(Vector2f);
	Cirno(Vector2f);
	~Cirno();
	void setMoveAnimation(Texture&, float);
	void setAttackAnimation(Texture&, float);
};