#pragma once
#include <SFML/Graphics.hpp>
#include "Projectile.h"
#include "CircleProjectile.h"
#include "Animation.h"

using namespace sf;

class Player
{
private:
	RectangleShape player;
	int currentAni;
	Animation* currentAnimation;
	Animation left;
	Animation right;
	Animation up;
	Animation down;
	Animation leftAttack;
	Animation rightAttack;
	Animation upAttack;
	Animation downAttack;

	Vector2f target;
	int hp;

	std::vector<std::shared_ptr<Projectile>> playerProjectiles;
	
	Animation rangeAnimation;
public:
	Player(Vector2f size, Vector2f pos)
	{
		player = RectangleShape(size);
		player.setPosition(pos);
		currentAnimation = &down;
		currentAni = 3;
	}

	~Player()
	{

	}

	void setMoveAnimation(Texture &t, float speed)
	{
		left = Animation(t, 128, 40, 6, 32, 40, speed);
		right = Animation(t, 64, 80, 6, 32, 40, speed);
		up = Animation(t, 0, 0, 6, 32, 40, speed);
		down = Animation(t, 192, 0, 6, 32, 40, speed);
	}

	void setAttackAnimation(Texture &t, float speed)
	{
		leftAttack = Animation(t, 64, 280, 5, 32, 40, speed);
		rightAttack = Animation(t, 224, 280, 5, 32, 40, speed);
		upAttack = Animation(t, 0, 240, 5, 32, 40, speed);
		downAttack = Animation(t, 160, 240, 5, 32, 40, speed);
	}

	void setRangeAnimation(Texture &t, float speed)
	{
		rangeAnimation = Animation(t, 128, 320, 4, 32, 48, speed);
	}

	void setCurrentAnimation(int i)
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

	void updatePosition()
	{
		currentAnimation->setPosition(player.getPosition());
	}

	void updateAnimation()
	{
		currentAnimation->update();
		if (currentAni >= 4 && currentAnimation->isOver())
		{
			playerProjectiles.push_back(shoot(target));
			setCurrentAnimation(currentAni - 4);
		}
	}

	Vector2f getPosition()
	{
		return player.getPosition();
	}

	Vector2f getSize()
	{
		return Vector2f(32, 40);
	}

	Sprite* getSprite()
	{
		return currentAnimation->getSprite();
	}

	std::shared_ptr<Projectile> shoot(Vector2f p)
	{
		std::shared_ptr<Projectile> cproj(new CircleProjectile(getPosition() + Vector2f(16,20), p));
		Vector2f v = p - (getPosition() + Vector2f(16, 20));
		Vector2f w(0, -1);
		float angle = atan2(determinant(v, w), dotProduct(v, w)) * 180.0 / 3.14159265;
		cproj->setAnimation(rangeAnimation, -angle);
		return cproj;
	}

	std::vector<std::shared_ptr<Projectile>>* getPlayerProjectiles()
	{
		return &playerProjectiles;
	}

	FloatRect getBounds()
	{
		return player.getGlobalBounds();
	}

	void draw(RenderWindow &window)
	{
		window.draw(*currentAnimation->getSprite());
	}

	void updateProjectile()
	{
		for (size_t i = 0; i < playerProjectiles.size(); ++i)
		{
			if (!playerProjectiles[i]->updateProjectile())
			{
				playerProjectiles.erase(playerProjectiles.begin() + i);
				--i;
			}
		}
	}

	void drawProjectile(RenderWindow &window)
	{
		for (const auto &p : playerProjectiles)
		{
			p->updateAnimation();
			p->updateSpritePosition();
			window.draw(*(p->getProjectile()));
			window.draw(*(p->getSprite()));
		}
	}

	void rangeAttack(Vector2f v)
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

	void move(Vector2f v, const std::vector<RectangleShape> &walls)
	{
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
};