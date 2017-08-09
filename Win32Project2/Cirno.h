#pragma once
#include "Enemy.h"
#include <stack>
#include "Projectile.h"
#include "CircleProjectile.h"
#include "AStar.h";

class Cirno : 
	public Enemy
{
private:
	RectangleShape cirno;
	Sprite sprite;

	int prevDirection;
	int currentSprite;
	
	Vector2f velocity;
	int counter;

	std::stack<int> path;
public:
	void spawn(Vector2f);
	std::shared_ptr<Projectile> shoot(Vector2f);
	Shape* getEnemy();
	void setSprite(Texture&);
	void updateSpriteNumber(int);
	void updateSprite();
	Sprite* getSprite();
	void enemyPathfinder(std::vector<double>&, Vector2f, std::vector<int>&);
	void updateEnemy();
	void clearStack();
	Cirno();
	~Cirno();
};