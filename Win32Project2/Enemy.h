#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;

class Projectile;

class Enemy
{
public:
	virtual std::shared_ptr<Projectile> shoot(Vector2f) = 0; //shoot in direction of player
	virtual Shape* getEnemy() = 0;
	virtual void setSprite(Texture&) = 0;
	virtual void updateSpriteNumber(int) = 0;
	virtual void updateSprite() = 0;
	virtual Sprite* getSprite() = 0;
	virtual void enemyPathfinder(std::vector<double>&, Vector2f, std::vector<int>&) = 0;
	virtual void updateEnemy(std::vector<std::shared_ptr<Enemy>>&) = 0;
	virtual void clearStack() = 0;
	virtual int getMoveState() = 0;
	Enemy();
	virtual ~Enemy();
};