#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

using namespace sf;

class Projectile
{
public:
	virtual void setAnimation(const Texture&, float) = 0;
	virtual void setAnimation(const Animation&, float) = 0;
	virtual Shape* getProjectile() = 0;
	virtual bool updateProjectile() = 0;
	virtual Sprite* getSprite() = 0;

	virtual void updateAnimation() = 0;
	virtual void updateSpritePosition() = 0;

	Projectile() = default;
	virtual ~Projectile() = default;
	Projectile(Projectile&&) = default;
	Projectile& operator=(Projectile&&) = default;
	Projectile(const Projectile&) = default;
	Projectile& operator=(const Projectile&) = default;
};