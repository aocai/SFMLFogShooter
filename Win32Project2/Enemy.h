#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include "Animation.h"
#include "AStar.h"

using namespace sf;

class Projectile;

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
public:
	virtual std::shared_ptr<Projectile> shoot(Vector2f) = 0; //shoot in direction of player
	virtual void enemyPathfinder(std::vector<double>&, Vector2f, std::vector<int>&);
	virtual void updateEnemy(std::vector<std::shared_ptr<Enemy>>&);
	virtual void targetReached();
	Enemy();
	virtual ~Enemy();

	FloatRect getBounds();
	Sprite* getSprite();
	int getMoveState();
	void updateAnimation();
	void updateSpritePosition();

	virtual void setMoveAnimation(Texture&, float) = 0;
	virtual void setAttackAnimation(Texture&, float) = 0;
	virtual void setCurrentAnimation(int);
};