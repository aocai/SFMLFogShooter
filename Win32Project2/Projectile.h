#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

using namespace sf;

class Projectile
{
protected:
	int projectileID;
	short projectileTypeID;
	Vector2f position;
	Vector2f velocity;
	Animation animation;
	bool over;

	float damage;
	static int IDCount;
public:
	virtual void setAnimation(const Texture&, float) = 0;
	virtual void setAnimation(const Animation&, float) = 0;
	const Sprite* getSprite() const;

	virtual bool updateProjectile() = 0;
	virtual void updateAnimation();
	virtual void updateSpritePosition();

	virtual float projDamageCalc(const FloatRect&) = 0;
	bool isOver() const;
	Vector2f getPosition() const;
	int getID() const;
	int getTypeID() const;
	static void incrementIDCount();

	virtual void draw(RenderWindow &window) = 0;

	Projectile(Vector2f, Vector2f, float);
	virtual ~Projectile() = default;
	Projectile(Projectile&&) = default;
	Projectile& operator=(Projectile&&) = default;
	Projectile(const Projectile&) = default;
	Projectile& operator=(const Projectile&) = default;
};