#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include "Animation.h"
#include "AStar.h"
#include "utilities.h"
#include "Projectile.h"
#include "Player.h"

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
	std::vector<Animation> animationVector = std::vector<Animation>(8);

	bool range;

	Vector2f target;
	std::vector<std::unique_ptr<Projectile>> enemyProjectile;

	mutable float hp;
	mutable RectangleShape hpBar;
public:
	virtual void shoot(Vector2f); //shoot in direction of player
	virtual void enemyPathfinder(const std::vector<double>&, Vector2f, std::vector<int>&);
	virtual void updateEnemy(const std::vector<std::unique_ptr<Enemy>>&);
	virtual void targetReached();

	Enemy(Vector2f);
	virtual ~Enemy() = default;
	Enemy(Enemy &&e) = default; //todo deepcopy
	Enemy& operator=(Enemy&&) = default;
	Enemy(const Enemy&) = default;
	Enemy& operator=(const Enemy&) = default;

	FloatRect getBounds() const;
	const Sprite* getSprite() const;
	int getMoveState() const;
	virtual void updateAnimation(const Player&);
	void updateSpritePosition();

	virtual void setMoveAnimation(const Texture&, float) = 0;
	virtual void setAttackAnimation(const Texture&, float) = 0;
	virtual void setCurrentAnimation(int);

	virtual void setRangeAnimation(const Texture&, float);
	virtual bool ranged() const;

	bool inRange(Vector2f) const;

	virtual void meleeAttack(Vector2f);
	virtual void rangeAttack(Vector2f);

	virtual void updateProjectile();
	void drawEnemy(RenderWindow&) const;
	void drawProjectiles(RenderWindow&) const;

	void calcProjCollision(const Player&);

	void takeDamage(float) const;
	float getCurrentHP() const;
};