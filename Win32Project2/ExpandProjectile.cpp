#include "ExpandProjectile.h"
#include "utilities.h"

const float ExpandProjectile::default_cooldown = 5.f;
const float ExpandProjectile::default_speed = 10.f;
const float ExpandProjectile::default_damage = 0.6f;

ExpandProjectile::ExpandProjectile(Vector2f p, Vector2f p2, float initialSize, float expandSize) :
	initialPosition(p), target(p2), finalSize(expandSize), Projectile(p, default_speed*(p2 - p) / magnitude(p2 - p), default_damage), totalDistance(magnitude(p2 - p)), scaling(1.05f, 1.05f), hitbox(initialSize)
{
	//hitbox = CircleShape(initialSize);
	hitbox.setOrigin(Vector2f(initialSize, initialSize));
	hitbox.setPosition(p);
	//hitbox.setFillColor(Color::Green);
	projectileTypeID = 2;
}

float ExpandProjectile::projDamageCalc(const FloatRect &bound)
{
	if (magnitude(hitbox.getPosition() - initialPosition) < totalDistance)
	{
		return 0;
	}
	if (hitbox.getGlobalBounds().intersects(bound))
	{
		return damage;
	}
	return 0;
}

void ExpandProjectile::setAnimation(const Texture &t, float speed)
{
	animation = Animation(t, 128, 320, 4, 32, 48, speed);
}

void ExpandProjectile::setAnimation(const Animation &a, float angle)
{
	animation = a;
	animation.setOrigin(Vector2f(16, 24));
	animation.scale(Vector2f(hitbox.getRadius() / 10.f, hitbox.getRadius() / 10.f));
	animation.setPosition(hitbox.getPosition());
}

bool ExpandProjectile::updateProjectile()
{
	if (magnitude(hitbox.getPosition() - initialPosition) >= totalDistance)
	{
		if (hitbox.getRadius()*hitbox.getScale().x <= 1)
		{
			over = true;
			return false;
		}
		if (hitbox.getRadius()*hitbox.getScale().x >= finalSize)
		{
			scaling = Vector2f(0.95f, 0.95f);
		}
		hitbox.scale(scaling);
		animation.scale(scaling);
	}
	else
	{
		position += velocity;
		//check for bounds
		if (position.x < 0 || position.x > 1280 || position.y < 0 || position.y > 720)
		{
			over = true;
			return false;
		}
		hitbox.move(velocity);
	}
	return true;
}

void ExpandProjectile::draw(RenderWindow &window)
{
	window.draw(*animation.getSprite());
}

const float ExpandProjectile::getCooldownTime()
{
	return default_cooldown;
}

Vector2f ExpandProjectile::getTarget() const
{
	return target;
}