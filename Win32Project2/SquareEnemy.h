#pragma once
#include "Enemy.h"
#include "CircleProjectile.h"

class SquareEnemy : 
	public Enemy
{
private:
	RectangleShape enemy;
	Sprite sprite;

	int prevDirection;
	int currentSprite;
	
	Vector2f velocity;
	int counter;
public:
	void spawn(Vector2f);
	std::shared_ptr<Projectile> shoot(Vector2f);
	Shape* getEnemy();
	void setSprite(Texture &texture);
	void updateSpriteNumber(int i);
	void updateSprite();
	Sprite* getSprite();
	void enemyPathfinder(Vector2f g, std::vector<int> &workVector);
	void updateEnemy();
	void clearStack();
	SquareEnemy();
	~SquareEnemy();
};