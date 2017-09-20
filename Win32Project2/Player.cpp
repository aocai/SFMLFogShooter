#include "Player.h"
#include "Enemy.h"

Player::Player(Vector2f size, Vector2f pos, int playerNumber) :
	playerID(playerNumber), hp(100), speed(5.f), currentAni(3), hitbox(size), hpBar(Vector2f(size.x, 5))
{
	//player = RectangleShape(size);
	hitbox.setPosition(pos);

	//hpBar = RectangleShape(Vector2f(size.x, 5));
	hpBar.setPosition(pos - Vector2f(0, 5));
	hpBar.setFillColor(Color::Red);
}

void Player::setMoveAnimation(const Texture &t, float speed)
{
	animationVector[0] = Animation(t, 128, 40, 6, 32, 40, speed);
	animationVector[1] = Animation(t, 64, 80, 6, 32, 40, speed);
	animationVector[2] = Animation(t, 0, 0, 6, 32, 40, speed);
	animationVector[3] = Animation(t, 192, 0, 6, 32, 40, speed);
}

void Player::setAttackAnimation(const Texture &t, float speed)
{
	animationVector[4] = Animation(t, 64, 280, 5, 32, 40, speed);
	animationVector[5] = Animation(t, 224, 280, 5, 32, 40, speed);
	animationVector[6] = Animation(t, 0, 240, 5, 32, 40, speed);
	animationVector[7] = Animation(t, 160, 240, 5, 32, 40, speed);
}

void Player::setRangeAnimation(const Texture &t, float speed)
{
	rangeAnimation = Animation(t, 128, 320, 4, 32, 48, speed);
}

void Player::setRangeAnimation2(const Texture &t, float speed)
{
	rangeAnimation2 = Animation(t, 128, 280, 6, 32, 40, speed);
}

void Player::setCurrentAnimation(int i)
{
	if (i != currentAni)
	{
		animationVector[currentAni].resetFrame();
		currentAni = i;
	}
}

//must be called after updateAnimation to set current animation position currently
void Player::updatePosition()
{
	animationVector[currentAni].setPosition(hitbox.getPosition());
}

void Player::updateAnimation()
{
	animationVector[currentAni].update();
	if (currentAni >= 4 && animationVector[currentAni].isOver())
	{
		shootStraight(target);
		setCurrentAnimation(currentAni - 4);
	}
}

void Player::setPosition(Vector2f v)
{
	hitbox.setPosition(v);
}

Vector2f Player::getPosition() const
{
	return hitbox.getPosition();
}

Vector2f Player::getSize() const
{
	return Vector2f(32, 40);
}

const Sprite* Player::getSprite() const
{
	return animationVector[currentAni].getSprite();
}

void Player::shootStraight(Vector2f p)
{
	if (clock.getElapsedTime().asSeconds() - cooldownCounters[0] > StraightProjectile::getCooldownTime())
	{
		std::unique_ptr<Projectile> cproj = std::make_unique<StraightProjectile>(getPosition() + Vector2f(16, 20), p, Vector2f(14, 40));
		Vector2f v = p - (getPosition() + Vector2f(16, 20));
		Vector2f w(0, -1);
		float angle = atan2(determinant(v, w), dotProduct(v, w)) * 180.0 / 3.14159265;
		cproj->setAnimation(rangeAnimation, -angle);
		playerProjectiles.emplace_back(std::move(cproj));
		cooldownCounters[0] = clock.getElapsedTime().asSeconds();
	}
}

bool Player::rangeAttack(Vector2f v)
{
	if (clock.getElapsedTime().asSeconds() - cooldownCounters[0] > StraightProjectile::getCooldownTime())
	{
		target = v;
		v = v - (getPosition() + Vector2f(16, 20));
		if (abs(v.y) > abs(v.x))
		{
			if (v.y < 0)
				setCurrentAnimation(6);
			else
				setCurrentAnimation(7);
		}
		else
		{
			if (v.x < 0)
				setCurrentAnimation(4);
			else
				setCurrentAnimation(5);
		}
		return true;
	}
	return false;
}

bool Player::shootSpiral()
{
	if (clock.getElapsedTime().asSeconds() - cooldownCounters[1] > SpiralProjectile::getCooldownTime())
	{
		std::unique_ptr<Projectile> cproj = std::make_unique<SpiralProjectile>(getPosition() + Vector2f(16, 20), 8.f);
		cproj->setAnimation(rangeAnimation2, 0);
		playerProjectiles.emplace_back(std::move(cproj));
		cooldownCounters[1] = clock.getElapsedTime().asSeconds();
		return true;
	}
	return false;
}

bool Player::shootExpand(Vector2f p)
{
	if (clock.getElapsedTime().asSeconds() - cooldownCounters[2] > ExpandProjectile::getCooldownTime())
	{
		std::unique_ptr<Projectile> cproj = std::make_unique<ExpandProjectile>(getPosition() + Vector2f(16, 20), p, 5.f, 30.f);
		cproj->setAnimation(rangeAnimation2, 0);
		playerProjectiles.emplace_back(std::move(cproj));
		cooldownCounters[2] = clock.getElapsedTime().asSeconds();
		return true;
	}
	return false;
}

FloatRect Player::getBounds() const
{
	return hitbox.getGlobalBounds();
}

void Player::draw(RenderWindow &window) const
{
	window.draw(*animationVector[currentAni].getSprite());
	window.draw(hpBar);
}

void Player::updateProjectile()
{
	for (size_t i = 0; i < playerProjectiles.size(); ++i)
	{
		if (!playerProjectiles[i]->updateProjectile())
		{
			playerProjectiles.erase(playerProjectiles.begin() + i);
			--i;
		}
		else
		{
			playerProjectiles[i]->updateAnimation();
			playerProjectiles[i]->updateSpritePosition();
		}
	}
}

void Player::drawProjectile(RenderWindow &window) const
{
	for (const auto &p : playerProjectiles)
	{
		p->draw(window);
	}
}

void Player::move(Vector2f v, const std::vector<RectangleShape> &walls)
{
	Vector2f diff = hitbox.getPosition();
	//move x component
	hitbox.move(Vector2f(v.x, 0));
	for (const auto &w : walls)
	{
		while (w.getGlobalBounds().intersects(hitbox.getGlobalBounds()))
		{
			if (v.x < 0)
				hitbox.move(Vector2f(1.0f, 0));
			else
				hitbox.move(Vector2f(-1.0f, 0));
		}
	}
	if (hitbox.getPosition().x < 0)
		hitbox.move(Vector2f(-hitbox.getPosition().x, 0));
	else if (hitbox.getPosition().x + hitbox.getSize().x > 1280)
		hitbox.move(Vector2f(1280 - (hitbox.getSize().x + hitbox.getPosition().x), 0));

	//move y component
	hitbox.move(Vector2f(0, v.y));
	for (const auto &w : walls)
	{
		while (w.getGlobalBounds().intersects(hitbox.getGlobalBounds()))
		{
			if (v.y < 0)
				hitbox.move(Vector2f(0, 1.0f));
			else
				hitbox.move(Vector2f(0, -1.0f));
		}
	}
	if (hitbox.getPosition().y < 0)
		hitbox.move(Vector2f(0, -hitbox.getPosition().y));
	else if (hitbox.getPosition().y + hitbox.getSize().y > 720)
		hitbox.move(Vector2f(0, 720 - (hitbox.getSize().y + hitbox.getPosition().y)));

	//move hpbar
	diff = hitbox.getPosition() - diff;
	hpBar.move(diff);

	if (v.x != 0)
	{
		if (v.x < 0)
			setCurrentAnimation(0);
		else
			setCurrentAnimation(1);
	}
	else
	{
		if (v.y < 0)
			setCurrentAnimation(2);
		else
			setCurrentAnimation(3);
	}
}

void Player::takeDamage(float dmg) const
{
	hpBar.scale(Vector2f(std::max(0.f,(hp - dmg)) / hp, 1));
	hp -= dmg;
}

float Player::getCurrentHP() const
{
	return hp;
}

void Player::calcProjCollision(const std::vector<std::unique_ptr<Enemy>> &e)
{
	for (int i = 0; i < playerProjectiles.size(); ++i)
	{
		for (auto& enemy : e)
		{
			float dealt = playerProjectiles[i]->projDamageCalc(enemy->getBounds());
			enemy->takeDamage(dealt);
			if (playerProjectiles[i]->isOver())
			{
				playerProjectiles.erase(playerProjectiles.begin() + i);
				--i;
				break;
			}
		}
	}
}

float Player::calcProjCollision(const sf::FloatRect &bounds)
{
	float dmg = 0;
	for (int i = 0; i < playerProjectiles.size(); ++i)
	{
		dmg += playerProjectiles[i]->projDamageCalc(bounds);
		if (playerProjectiles[i]->isOver())
		{
			playerProjectiles.erase(playerProjectiles.begin() + i);
			--i;
		}
	}
	return dmg;
}

float Player::getSpeed() const
{
	return speed;
}

int Player::getID() const
{
	return playerID;
}

const std::vector<std::unique_ptr<Projectile>>* Player::getProjectiles() const
{
	return &playerProjectiles;
}