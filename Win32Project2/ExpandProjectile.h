#pragma once
#include "Projectile.h"
#include "utilities.h"

class ExpandProjectile
	: public Projectile
{
private:
	CircleShape hitbox;
	Vector2f initialPosition;
	Vector2f target;
	float finalSize;
	float totalDistance;
	Vector2f scaling;
	static const float default_cooldown;
	static const float default_speed;
	static const float default_damage;
public:
	ExpandProjectile(Vector2f, Vector2f, float, float);
	float projDamageCalc(const FloatRect&) override;
	void setAnimation(const Texture&, float) override;
	void setAnimation(const Animation&, float) override;
	bool updateProjectile() override;
	void draw(RenderTarget &target) override;
	static const float getCooldownTime();
	Vector2f getTarget() const;
};