#include "Projectile.h"

Sprite* Projectile::getSprite()
{
	return animation.getSprite();
}

void Projectile::updateAnimation()
{
	animation.update();
}

void Projectile::updateSpritePosition()
{
	animation.getSprite()->setPosition(position);
}

bool Projectile::isOver() const
{
	return over;
}