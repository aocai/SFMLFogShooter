#pragma once
#include "Enemy.h"

class SquareEnemy : 
	public Enemy
{
public:
	RectangleShape re;
	void spawn(Vector2f position);
	void destroy();
	void shoot();
	SquareEnemy();
	~SquareEnemy();
};