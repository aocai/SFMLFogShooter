#pragma once
#include "Projectile.h"
#include "utilities.h"

class CircleProjectile
	: public Projectile
{
private:
	CircleShape ci;
	Vector2f position;
	Vector2f velocity;
	Animation animation;
public:
	CircleProjectile();
	CircleProjectile(Vector2f, Vector2f);
	void setAnimation(Texture&, float);
	Shape* getProjectile();
	bool updateProjectile();
	~CircleProjectile();
};