#pragma once
#include "Projectile.h"
#include "utilities.h"

class CircleProjectile
	: public Projectile
{
private:
	RectangleShape hitbox;
	Vector2f position;
	Vector2f velocity;
	Animation animation;
public:
	CircleProjectile(Vector2f, Vector2f);
	void setAnimation(const Texture&, float);
	void setAnimation(const Animation&, float);
	Shape* getProjectile();
	bool updateProjectile();
	Sprite* getSprite();

	void updateAnimation();
	void updateSpritePosition();
};