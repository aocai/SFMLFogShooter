#pragma once
#include "Projectile.h"
#include "utilities.h"

class StraightProjectile
	: public Projectile
{
private:
	RectangleShape hitbox;
	static const float cooldown;
	static const float speed;
public:
	StraightProjectile(Vector2f, Vector2f, Vector2f);
	float projDamageCalc(const FloatRect&) override;
	void setAnimation(const Texture&, float) override;
	void setAnimation(const Animation&, float) override;
	bool updateProjectile() override;
	void draw(RenderWindow &window) override;
	static const float getCooldownTime();
};