#include "SpiralProjectile.h"

const float SpiralProjectile::default_cooldown = 10.f;
const float SpiralProjectile::default_speed = 5.f;
const float SpiralProjectile::default_damage = 5.f;

SpiralProjectile::SpiralProjectile(Vector2f p, float size) : 
	hitboxes(18), notValidMask(0), Projectile(p,Vector2f(0,-default_speed), default_damage)
{
	for (int i = 0; i < 18; ++i)
	{
		hitboxes[i].setRadius(size);
		hitboxes[i].setOrigin(Vector2f(size, size));
		hitboxes[i].setPosition(p);
		//hitboxes[i].setFillColor(Color::Green);
		hitboxes[i].setRotation(i*20.f);
	}
	projectileTypeID = 1;
}

float SpiralProjectile::projDamageCalc(const FloatRect &bound)
{
	int count = 0;
	for (int i = 0; i < hitboxes.size(); ++i)
	{
		if (notValid(i))
			continue;
		if (hitboxes[i].getGlobalBounds().intersects(bound))
		{
			//hitboxes.erase(hitboxes.begin() + i);
			//--i;
			notValidMask |= (1 << i);
			++count;
		}
	}
	if (notValidMask == (1 << 18) - 1)
	{
		over = true;
	}
	return count * damage;
}

void SpiralProjectile::setAnimation(const Texture &t, float speed)
{
	animation = Animation(t, 128, 320, 4, 32, 48, speed);
}

void SpiralProjectile::setAnimation(const Animation &a, float angle)
{
	animation = a;
	animation.setOrigin(Vector2f(16,24));
}

bool SpiralProjectile::updateProjectile()
{
	for (int i = 0; i < hitboxes.size(); ++i)
	{
		if (notValid(i))
			continue;
		hitboxes[i].move(rotateVector2f(velocity, hitboxes[i].getRotation()));
		hitboxes[i].setRotation(hitboxes[i].getRotation() + .5f);
		//check for bounds
		Vector2f p = hitboxes[i].getPosition();
		if (p.x < 0 || p.x > 1280 || p.y < 0 || p.y > 720)
		{
			//hitboxes.erase(hitboxes.begin() + i);
			//--i;
			notValidMask |= (1 << i);
		}
	}
	if (notValidMask == (1 << 18) - 1)
	{
		over = true;
		return false;
	}
	return true;
}

void SpiralProjectile::draw(RenderWindow &window)
{
	for (int i = 0; i < hitboxes.size(); ++i)
	{
		if (notValid(i))
			continue;
		animation.setPosition(hitboxes[i].getPosition());
		window.draw(*animation.getSprite());
	}
}

const float SpiralProjectile::getCooldownTime()
{
	return default_cooldown;
}

//check if current sub-projectile index is still alive/valid
bool SpiralProjectile::notValid(int i) const
{
	return (1 << i) & notValidMask;
}

long long SpiralProjectile::getNotValidMask() const
{
	return notValidMask;
}

void SpiralProjectile::setNotValidMask(long long mask)
{
	notValidMask = mask;
}