#include "StraightProjectile.h"

StraightProjectile::StraightProjectile(Vector2f p, Vector2f p2, Vector2f hitSize)
{
	hitbox = RectangleShape(hitSize);
	hitbox.setOrigin(hitSize /2.f);
	hitbox.setPosition(p);
	hitbox.setFillColor(Color::Green);
	position = p;
	Vector2f v = p2 - p;
	velocity = Vector2f(5.0f * (v / magnitude(v)));
	dmg = 10.f;
	over = false;
}

float StraightProjectile::projDamageCalc(const FloatRect& bound)
{
	if (hitbox.getGlobalBounds().intersects(bound))
	{
		over = true;
		return dmg;
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
	animation.getSprite()->setOrigin(Vector2f(16,20));
	animation.setPosition(hitbox.getPosition());
	animation.getSprite()->rotate(angle);
	hitbox.rotate(angle);
}

bool StraightProjectile::updateProjectile()
{
	position += velocity;
	//check for bounds
	if (position.x < 0 || position.x > 1280 || position.y < 0 || position.y > 720)
	{
		return false;
	}
	hitbox.move(velocity);
	return true;
}

void StraightProjectile::draw(RenderWindow &window)
{
	window.draw(hitbox);
	window.draw(*animation.getSprite());
}