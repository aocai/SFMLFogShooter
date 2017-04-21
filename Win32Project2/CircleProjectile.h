#pragma once
#include "Projectile.h"

class CircleProjectile
	: public Projectile
{
public:
	CircleShape ci;
	Vector2f position;
	Vector2f velocity;
	void spawn(Vector2f p, Vector2f n);
	void destroy();
	CircleProjectile();
	~CircleProjectile();
};