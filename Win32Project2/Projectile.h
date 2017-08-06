#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Projectile
{
public:
	Vector2f position;
	Vector2f velocity;
	virtual void spawn(Vector2f, Vector2f) = 0;
	virtual Shape* getProjectile() = 0;
	Projectile();
	virtual ~Projectile();
};