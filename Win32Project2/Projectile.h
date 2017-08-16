#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

using namespace sf;

class Projectile
{
public:
	Projectile();
	virtual void setAnimation(Texture&, float) = 0;
	virtual void setAnimation(Animation, float) = 0;
	virtual Shape* getProjectile() = 0;
	virtual bool updateProjectile() = 0;
	virtual Sprite* getSprite() = 0;

	virtual void updateAnimation() = 0;
	virtual void updateSpritePosition() = 0;

	virtual ~Projectile();
};