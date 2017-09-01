#pragma once
#include "Enemy.h"

class Aya :
	public Enemy
{
public:
	Aya(Vector2f);
	void setMoveAnimation(const Texture&, float) override;
	void setAttackAnimation(const Texture&, float) override;
};