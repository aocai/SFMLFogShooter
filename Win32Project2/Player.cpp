#include "Player.h"
#include "Enemy.h"

Player::Player(Vector2f size, Vector2f pos)
{
	player = RectangleShape(size);
	player.setPosition(pos);
	currentAnimation = &down;
	currentAni = 3;
	hp = 100;
	hpBar = RectangleShape(Vector2f(size.x, 5));
	hpBar.setPosition(pos - Vector2f(0, 5));
	hpBar.setFillColor(Color::Red);
}

void Player::setMoveAnimation(const Texture &t, float speed)
{
	left = Animation(t, 128, 40, 6, 32, 40, speed);
	right = Animation(t, 64, 80, 6, 32, 40, speed);
	up = Animation(t, 0, 0, 6, 32, 40, speed);
	down = Animation(t, 192, 0, 6, 32, 40, speed);
}

void Player::setAttackAnimation(const Texture &t, float speed)
{
	leftAttack = Animation(t, 64, 280, 5, 32, 40, speed);
	rightAttack = Animation(t, 224, 280, 5, 32, 40, speed);
	upAttack = Animation(t, 0, 240, 5, 32, 40, speed);
	downAttack = Animation(t, 160, 240, 5, 32, 40, speed);
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
	if (i == 0 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &left;
		currentAni = 0;
	}
	else if (i == 1 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &right;
		currentAni = 1;
	}
	else if (i == 2 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &up;
		currentAni = 2;
	}
	else if (i == 3 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &down;
		currentAni = 3;
	}
	else if (i == 4 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &leftAttack;
		currentAni = 4;
	}
	else if (i == 5 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &rightAttack;
		currentAni = 5;
	}
	else if (i == 6 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &upAttack;
		currentAni = 6;
	}
	else if (i == 7 && i != currentAni)
	{
		currentAnimation->resetFrame();
		currentAnimation = &downAttack;
		currentAni = 7;
	}
}

void Player::updatePosition()
{
	currentAnimation->setPosition(player.getPosition());
}

void Player::updateAnimation()
{
	currentAnimation->update();
	if (currentAni >= 4 && currentAnimation->isOver())
	{
		shootStraight(target);
		setCurrentAnimation(currentAni - 4);
	}
}

Vector2f Player::getPosition() const
{
	return player.getPosition();
}

Vector2f Player::getSize() const
{
	return Vector2f(32, 40);
}

Sprite* Player::getSprite()
{
	return currentAnimation->getSprite();
}

void Player::shootStraight(Vector2f p)
{
	std::unique_ptr<Projectile> cproj = std::make_unique<StraightProjectile>(getPosition() + Vector2f(16, 20), p, Vector2f(14,40));
	Vector2f v = p - (getPosition() + Vector2f(16, 20));
	Vector2f w(0, -1);
	float angle = atan2(determinant(v, w), dotProduct(v, w)) * 180.0 / 3.14159265;
	cproj->setAnimation(rangeAnimation, -angle);
	playerProjectiles.emplace_back(std::move(cproj));
}

void Player::shootSpiral()
{
	std::unique_ptr<Projectile> cproj = std::make_unique<SpiralProjectile>(getPosition() + Vector2f(16, 20), 8.f);
	cproj->setAnimation(rangeAnimation2, 0);
	playerProjectiles.emplace_back(std::move(cproj));
}

void Player::shootExpand(Vector2f p)
{
	std::unique_ptr<Projectile> cproj = std::make_unique<ExpandProjectile>(getPosition() + Vector2f(16, 20), p, 5.f, 30.f);
	cproj->setAnimation(rangeAnimation2, 0);
	playerProjectiles.emplace_back(std::move(cproj));
}

FloatRect Player::getBounds() const
{
	return player.getGlobalBounds();
}

void Player::draw(RenderWindow &window) const
{
	window.draw(*currentAnimation->getSprite());
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

void Player::rangeAttack(Vector2f v)
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
}

void Player::move(Vector2f v, const std::vector<RectangleShape> &walls)
{
	Vector2f diff = player.getPosition();
	//move x component
	player.move(Vector2f(v.x, 0));
	for (const auto &w : walls)
	{
		while (w.getGlobalBounds().intersects(player.getGlobalBounds()))
		{
			if (v.x < 0)
				player.move(Vector2f(1.0f, 0));
			else
				player.move(Vector2f(-1.0f, 0));
		}
	}
	if (player.getPosition().x < 0)
		player.move(Vector2f(-player.getPosition().x, 0));
	else if (player.getPosition().x + player.getSize().x > 1280)
		player.move(Vector2f(1280 - (player.getSize().x + player.getPosition().x), 0));

	//move y component
	player.move(Vector2f(0, v.y));
	for (const auto &w : walls)
	{
		while (w.getGlobalBounds().intersects(player.getGlobalBounds()))
		{
			if (v.y < 0)
				player.move(Vector2f(0, 1.0f));
			else
				player.move(Vector2f(0, -1.0f));
		}
	}
	if (player.getPosition().y < 0)
		player.move(Vector2f(0, -player.getPosition().y));
	else if (player.getPosition().y + player.getSize().y > 720)
		player.move(Vector2f(0, 720 - (player.getSize().y + player.getPosition().y)));

	//move hpbar
	diff = player.getPosition() - diff;
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
