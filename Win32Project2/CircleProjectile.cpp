#include "CircleProjectile.h"

CircleProjectile::CircleProjectile(Vector2f p, Vector2f p2)
{
	hitbox = RectangleShape(Vector2f(20, 48));
	hitbox.setOrigin(Vector2f(10, 24));
	hitbox.setPosition(p);
	hitbox.setFillColor(Color::Green);
	position = p;
	Vector2f v = p2 - p;
	velocity = Vector2f(5.0f * (v / magnitude(v)));
}

void CircleProjectile::setAnimation(Texture &t, float speed)
{
	animation = Animation(t, 128, 320, 4, 32, 48, speed);
}

void CircleProjectile::setAnimation(Animation a, float angle)
{
	animation = a;
	animation.getSprite()->setOrigin(Vector2f(16,20));
	animation.setPosition(hitbox.getPosition());
	animation.getSprite()->rotate(angle);
	hitbox.rotate(angle);
}

Shape* CircleProjectile::getProjectile()
{
	return &hitbox;
}

bool CircleProjectile::updateProjectile()
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

Sprite* CircleProjectile::getSprite()
{
	return animation.getSprite();
}

void CircleProjectile::updateAnimation()
{
	animation.update();
}

void CircleProjectile::updateSpritePosition()
{
	animation.getSprite()->setPosition(position);
}