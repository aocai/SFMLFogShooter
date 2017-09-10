#pragma once
#include "Projectile.h"
#include "utilities.h"

class ExpandProjectile
	: public Projectile
{
private:
	CircleShape hitbox;
	Vector2f initialPosition;
	float finalSize;
	float totalDistance;
	Vector2f scaling;
	static const float cooldown;
	static const float speed;
public:
	ExpandProjectile(Vector2f, Vector2f, float, float);
	float projDamageCalc(const FloatRect&) override;
	void setAnimation(const Texture&, float) override;
	void setAnimation(const Animation&, float) override;
	bool updateProjectile() override;
	void draw(RenderWindow &window) override;
	static const float getCooldownTime();
};