#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Enemy
{
public:
	Vector2f position;
	Vector2f normal;
	virtual void spawn(Vector2f position);
	virtual void destroy();
	virtual void shoot(); //shoot in direction of normal
	Enemy();
	~Enemy();
};