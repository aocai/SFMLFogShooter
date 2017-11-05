#pragma once
#include "Projectile.h"
#include "utilities.h"

class SpiralProjectile
	: public Projectile
{
private:
	std::vector<CircleShape> hitboxes;
	long long notValidMask; //bit 0 = valid, bit 1 = not valid
	static const float default_cooldown;
	static const float default_speed;
	static const float default_damage;
public:
	SpiralProjectile(Vector2f, float);
	float projDamageCalc(const FloatRect&) override;
	void setAnimation(const Texture&, float) override;
	void setAnimation(const Animation&, float) override;
	bool updateProjectile() override;
	void draw(RenderTarget &target) override;
	static const float getCooldownTime();
	bool notValid(int i) const;
	long long getNotValidMask() const;
	void setNotValidMask(long long);
};