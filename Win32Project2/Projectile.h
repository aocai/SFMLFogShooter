#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Projectile
{
public:
	Vector2f position;
	Vector2f velocity;
	virtual void spawn(Vector2f position, Vector2f normal);
	virtual void destroy();
	Projectile();
	~Projectile();
};