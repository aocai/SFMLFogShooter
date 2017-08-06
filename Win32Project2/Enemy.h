#pragma once
#include <SFML/Graphics.hpp>
#include "Projectile.h"
#include <stack>
#include "main.h"

using namespace sf;

class Enemy
{
public:
	std::stack<int> path;
	virtual void spawn(Vector2f) = 0;
	virtual std::shared_ptr<Projectile> shoot(Vector2f) = 0; //shoot in direction of player
	virtual Shape* getEnemy() = 0;
	virtual void setSprite(Texture &texture) = 0;
	virtual void updateSpriteNumber(int i) = 0;
	virtual void updateSprite() = 0;
	virtual Sprite* getSprite() = 0;
	virtual void enemyPathfinder(Vector2f, std::vector<int>&) = 0;
	virtual void updateEnemy() = 0;
	virtual void clearStack() = 0;
	Enemy();
	virtual ~Enemy();
};