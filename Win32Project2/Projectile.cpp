#include "Projectile.h"
#include "StraightProjectile.h"

int Projectile::IDCount = 0;

Projectile::Projectile(Vector2f p, Vector2f v, float dmg, int pType) :
	position(p), velocity(v), over(false), damage(dmg), projectileTypeID(pType), projectileID(IDCount)
{

}

const Sprite* Projectile::getSprite() const
{
	return animation.getSprite();
}

void Projectile::updateAnimation()
{
	animation.update();
}

void Projectile::updateSpritePosition()
{
	animation.setPosition(position);
}

bool Projectile::isOver() const
{
	return over;
}

Vector2f Projectile::getPosition() const
{
	return animation.getSprite()->getPosition();
}

int Projectile::getID() const
{
	return projectileID;
}

int Projectile::getTypeID() const
{
	return projectileTypeID;
}

void Projectile::incrementIDCount()
{
	++IDCount;
}