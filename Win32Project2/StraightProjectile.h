#pragma once
#include "Projectile.h"
#include "utilities.h"

class StraightProjectile
	: public Projectile
{
private:
	RectangleShape hitbox;
	Vector2f target;
	static const float default_cooldown;
	static const float default_speed;
	static const float default_damage;
public:
	StraightProjectile(Vector2f, Vector2f, Vector2f);
	float projDamageCalc(const FloatRect&) override;
	void setAnimation(const Texture&, float) override;
	void setAnimation(const Animation&, float) override;
	bool updateProjectile() override;
	void draw(RenderTarget &target) override;
	static const float getCooldownTime();
	Vector2f getTarget() const;
};