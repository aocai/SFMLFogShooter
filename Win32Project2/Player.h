#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <chrono>
#include "Projectile.h"
#include "StraightProjectile.h"
#include "SpiralProjectile.h"
#include "ExpandProjectile.h"
#include "Animation.h"

using namespace sf;

class Enemy;

struct PlayerFrameInput
{
	char playerID;
	float deltaX;
	float deltaY;
	int SkillUsedMask; //0 bit = straightproj, 1 bit = spiralproj, 2 bit = expandproj used
	float targetX;
	float targetY;
};

typedef std::chrono::time_point<std::chrono::system_clock> sysclock;

class Player
{
private:
	RectangleShape hitbox;
	int playerID;
	float speed;
	int currentAni;
	std::vector<Animation> animationVector = std::vector<Animation>(8);

	std::vector<std::unique_ptr<Projectile>> playerProjectiles;
	
	Animation rangeAnimation;
	Animation rangeAnimation2;

	mutable float hp;
	mutable RectangleShape hpBar;

	//sysclock clockStart = std::chrono::system_clock::now();
	std::vector<sysclock> cooldownCounter = std::vector<sysclock>(3, sysclock());
public:
	Player(Vector2f size, Vector2f pos, int playerNumber = 0);
	void setMoveAnimation(const Texture &t, float speed);
	void setAttackAnimation(const Texture &t, float speed);
	void setRangeAnimation(const Texture &t, float speed);
	void setRangeAnimation2(const Texture &t, float speed);
	void setCurrentAnimation(int i);
	void setPosition(Vector2f);
	void updatePosition();
	void updateAnimation();
	Vector2f getPosition() const;
	Vector2f getSize() const;
	const Sprite* getSprite() const;
	bool isCooledDown(int) const;
	bool shootStraight(Vector2f p);
	bool startRangeAttackAnimation(Vector2f v);
	bool shootSpiral();
	bool shootExpand(Vector2f);
	FloatRect getBounds() const;
	void draw(RenderTarget &target) const;
	void updateProjectile();
	void drawProjectile(RenderTarget &target) const;
	void move(Vector2f v, const std::vector<RectangleShape> &walls);
	void takeDamage(float dmg) const;
	float getCurrentHP() const;
	void calcProjCollision(const std::vector<std::unique_ptr<Enemy>> &e);
	float calcProjCollision(const sf::FloatRect &bounds);
	float getSpeed() const;
	int getID() const;
	const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const;

	Player() = default;
	Player(Player &&e) = default;
	Player& operator=(Player&&) = default;
};