#include "SpiralProjectile.h"

SpiralProjectile::SpiralProjectile(Vector2f p, float size) : hitboxes(18)
{
	position = p;
	velocity = Vector2f(0, -5);
	dmg = 5.f;
	over = false;
	for (int i = 0; i < 18; ++i)
	{
		hitboxes[i].setRadius(size);
		hitboxes[i].setOrigin(Vector2f(size, size));
		hitboxes[i].setPosition(p);
		hitboxes[i].setFillColor(Color::Green);
		hitboxes[i].setRotation(i*20.f);
	}
}

float SpiralProjectile::projDamageCalc(const FloatRect &bound)
{
	int count = 0;
	for (int i = 0; i < hitboxes.size(); ++i)
	{
		if (hitboxes[i].getGlobalBounds().intersects(bound))
		{
			hitboxes.erase(hitboxes.begin() + i);
			--i;
			++count;
		}
	}
	if (hitboxes.empty())
	{
		over = true;
	}
	return count * dmg;
}

void SpiralProjectile::setAnimation(const Texture &t, float speed)
{
	animation = Animation(t, 128, 320, 4, 32, 48, speed);
}

void SpiralProjectile::setAnimation(const Animation &a, float angle)
{
	animation = a;
	animation.getSprite()->setOrigin(Vector2f(16,24));
}

bool SpiralProjectile::updateProjectile()
{
	for (int i = 0; i < hitboxes.size(); ++i)
	{
		hitboxes[i].move(rotateVector2f(velocity, hitboxes[i].getRotation()));
		hitboxes[i].setRotation(hitboxes[i].getRotation() + .5f);
		//check for bounds
		Vector2f p = hitboxes[i].getPosition();
		if (p.x < 0 || p.x > 1280 || p.y < 0 || p.y > 720)
		{
			hitboxes.erase(hitboxes.begin() + i);
			--i;
		}
	}
	if (hitboxes.empty())
	{
		over = true;
		return false;
	}
	return true;
}

void SpiralProjectile::draw(RenderWindow &window)
{
	for (auto &i : hitboxes)
	{
		animation.setPosition(i.getPosition());
		window.draw(i); //test
		window.draw(*animation.getSprite());
	}
}