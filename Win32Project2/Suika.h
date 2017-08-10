#pragma once
#include "Enemy.h"
#include <stack>
#include "Projectile.h"
#include "CircleProjectile.h"
#include "AStar.h";

class Suika :
	public Enemy
{
private:
	RectangleShape suika;
	Sprite sprite;
	RectangleShape collisionBox;
	int moveState;

	int prevDirection;
	int currentSprite;

	Vector2f velocity;
	int counter;

	std::stack<int> path;
public:
	std::shared_ptr<Projectile> shoot(Vector2f);
	Shape* getEnemy();
	void setSprite(Texture&);
	void updateSpriteNumber(int);
	void updateSprite();
	Sprite* getSprite();
	void enemyPathfinder(std::vector<double>&, Vector2f, std::vector<int>&);
	void updateEnemy(std::vector<std::shared_ptr<Enemy>>&);
	void clearStack();
	RectangleShape* getCollisionBox();
	int getMoveState();
	Suika(Vector2f);
	~Suika();
};