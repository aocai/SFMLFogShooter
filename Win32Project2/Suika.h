#pragma once
#include "Enemy.h"

class Suika :
	public Enemy
{
public:
	Suika(Vector2f);
	void setMoveAnimation(const Texture&, float) override;
	void setAttackAnimation(const Texture&, float) override;
};