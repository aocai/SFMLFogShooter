#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Projectile.h"
#include "StraightProjectile.h"
#include "SpiralProjectile.h"
#include "ExpandProjectile.h"
#include "Animation.h"

using namespace sf;

class Enemy;

class Player
{
private:
	RectangleShape player;
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

	Vector2f target;

	std::vector<std::unique_ptr<Projectile>> playerProjectiles;
	
	Animation rangeAnimation;
	Animation rangeAnimation2;

	mutable float hp;
	mutable RectangleShape hpBar;
public:
	Player(Vector2f size, Vector2f pos);
	void setMoveAnimation(const Texture &t, float speed);
	void setAttackAnimation(const Texture &t, float speed);
	void setRangeAnimation(const Texture &t, float speed);
	void setRangeAnimation2(const Texture &t, float speed);
	void setCurrentAnimation(int i);
	void updatePosition();
	void updateAnimation();
	Vector2f getPosition() const;
	Vector2f getSize() const;
	Sprite* getSprite();
	void shootStraight(Vector2f p);
	void shootSpiral();
	void shootExpand(Vector2f);
	FloatRect getBounds() const;
	void draw(RenderWindow &window) const;
	void updateProjectile();
	void drawProjectile(RenderWindow &window) const;
	void rangeAttack(Vector2f v);
	void move(Vector2f v, const std::vector<RectangleShape> &walls);
	void takeDamage(float dmg) const;
	float getCurrentHP() const;
	void calcProjCollision(const std::vector<std::unique_ptr<Enemy>> &e);
	float calcProjCollision(const sf::FloatRect &bounds);
};