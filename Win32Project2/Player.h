#pragma once
#include <SFML/Graphics.hpp>
#include "Projectile.h"
#include "CircleProjectile.h"
#include "Animation.h"

using namespace sf;

class Player
{
private:
	//Vector2f position;
	RectangleShape playerBox;
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
public:
	Player(Vector2f &size, Vector2f &pos)
	{
		//position = pos + Vector2f(size.x/2, size.y/2);
		playerBox.setSize(size);
		playerBox.setPosition(pos);
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
		return playerBox.getPosition();
	}

	RectangleShape* getPlayer()
	{
		return &playerBox;
	}

	Sprite* getSprite()
	{
		return currentAnimation->getSprite();
	}

	void updateSpritePosition()
	{
		currentAnimation->getSprite()->setPosition(playerBox.getPosition());
	}

	void updateTarget(Vector2f v)
	{
		target = v;
	}

	std::shared_ptr<Projectile> shoot(Vector2f p)
	{
		std::shared_ptr<Projectile> cproj(new CircleProjectile(playerBox.getPosition() + playerBox.getSize() / 2.f, p));
		return cproj;
	}

	std::vector<std::shared_ptr<Projectile>>* getPlayerProjectiles()
	{
		return &playerProjectiles;
	}
};