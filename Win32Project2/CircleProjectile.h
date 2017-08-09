#pragma once
#include "Projectile.h"
#include "utilities.h"

class CircleProjectile
	: public Projectile
{
private:
	CircleShape ci;
public:
	void spawn(Vector2f, Vector2f);
	Shape* getProjectile();
	CircleProjectile();
	~CircleProjectile();
};