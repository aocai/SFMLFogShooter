#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

using namespace sf;

class Projectile
{
protected:
	Vector2f position;
	Vector2f velocity;
	Animation animation;
	float dmg;
	bool over;
public:
	virtual void setAnimation(const Texture&, float) = 0;
	virtual void setAnimation(const Animation&, float) = 0;
	virtual Sprite* getSprite();

	virtual bool updateProjectile() = 0;
	virtual void updateAnimation();
	virtual void updateSpritePosition();

	virtual float projDamageCalc(const FloatRect&) = 0;
	bool isOver() const;

	virtual void draw(RenderWindow &window) = 0;

	Projectile() = default;
	virtual ~Projectile() = default;
	Projectile(Projectile&&) = default;
	Projectile& operator=(Projectile&&) = default;
	Projectile(const Projectile&) = default;
	Projectile& operator=(const Projectile&) = default;
};