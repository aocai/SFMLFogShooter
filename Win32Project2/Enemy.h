#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include "Animation.h"
#include "AStar.h"
#include "utilities.h"
#include "Projectile.h"

using namespace sf;

class Enemy
{
protected:
	RectangleShape character;
	Vector2f velocity;
	int counter;

	std::stack<int> path;
	int moveState;

	int currentAni;
	Animation* currentAnimation;
	Animation left;
	Animation right;
	Animation up;
	Animation down;
	Animation leftAttack;
	Animation rightAttack;
	Animation upAttack;
	Animation downAttack;

	bool range;

	Vector2f target;
	std::vector<std::shared_ptr<Projectile>> enemyProjectile;
public:
	virtual std::shared_ptr<Projectile> shoot(Vector2f) = 0; //shoot in direction of player
	virtual void enemyPathfinder(std::vector<double>&, Vector2f, std::vector<int>&);
	virtual void updateEnemy(std::vector<std::shared_ptr<Enemy>>&);
	virtual void targetReached();
	virtual ~Enemy() = default;

	FloatRect getBounds();
	Sprite* getSprite();
	int getMoveState();
	virtual void updateAnimation();
	void updateSpritePosition();

	virtual void setMoveAnimation(Texture&, float) = 0;
	virtual void setAttackAnimation(Texture&, float) = 0;
	virtual void setCurrentAnimation(int);

	virtual void setRangeAnimation(Texture&, float);
	virtual bool ranged();

	bool inRange(Vector2f);

	virtual void meleeAttack(Vector2f);
	virtual void rangeAttack(Vector2f);

	virtual void updateProjectile();
	void drawEnemy(RenderWindow&);
	void drawProjectiles(RenderWindow&);
};