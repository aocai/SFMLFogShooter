#include "StraightProjectile.h"

const float StraightProjectile::default_cooldown = 0.f;
const float StraightProjectile::default_speed = 8.f;
const float StraightProjectile::default_damage = 10.f;

StraightProjectile::StraightProjectile(Vector2f p, Vector2f p2, Vector2f hitSize) : 
	target(p2), Projectile(p, default_speed*(p2-p)/magnitude(p2-p), default_damage, 0), hitbox(hitSize)
{
	//hitbox = RectangleShape(hitSize);
	hitbox.setOrigin(hitSize / 2.f);
	hitbox.setPosition(p);
	//hitbox.setFillColor(Color::Green);
}

float StraightProjectile::projDamageCalc(const FloatRect& bound)
{
	if (hitbox.getGlobalBounds().intersects(bound))
	{
		over = true;
		return damage;
	}
	return 0;
}

void StraightProjectile::setAnimation(const Texture &t, float speed)
{
	animation = Animation(t, 128, 320, 4, 32, 48, speed);
}

void StraightProjectile::setAnimation(const Animation &a, float angle)
{
	animation = a;
	animation.setOrigin(Vector2f(16,20));
	animation.setPosition(hitbox.getPosition());
	animation.rotate(angle);
	hitbox.rotate(angle);
}

bool StraightProjectile::updateProjectile()
{
	position += velocity;
	//check for bounds
	if (position.x < 0 || position.x > 1280 || position.y < 0 || position.y > 720)
	{
		over = true;
		return false;
	}
	hitbox.move(velocity);
	return true;
}

void StraightProjectile::draw(RenderTarget &target)
{
	target.draw(*animation.getSprite());
}

const float StraightProjectile::getCooldownTime()
{
	return default_cooldown;
}

Vector2f StraightProjectile::getTarget() const
{
	return target;
}