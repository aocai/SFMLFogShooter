#pragma once
#include "Projectile.h"
#include "utilities.h"

class SpiralProjectile
	: public Projectile
{
private:
	std::vector<CircleShape> hitboxes;
	static const float cooldown;
public:
	SpiralProjectile(Vector2f, float);
	float projDamageCalc(const FloatRect&) override;
	void setAnimation(const Texture&, float) override;
	void setAnimation(const Animation&, float) override;
	bool updateProjectile() override;
	void draw(RenderWindow &window) override;
	static const float getCooldownTime();
};