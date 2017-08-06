#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
private:
	//Vector2f position;
	RectangleShape playerBox;
	Sprite sprite;

	int currentSprite;
	int attackSprite;
	int hp;
public:
	Player(Vector2f &size, Vector2f &pos)
	{
		//position = pos + Vector2f(size.x/2, size.y/2);
		playerBox.setSize(size);
		playerBox.setPosition(pos);
	}

	~Player()
	{

	}

	Vector2f getPosition()
	{
		return playerBox.getPosition();
	}

	RectangleShape* getPlayer()
	{
		return &playerBox;
	}

	void setSprite(Texture &texture)
	{
		IntRect playerSpriteRect(192, 0, 32, 40);
		sprite = Sprite(texture, playerSpriteRect);
		sprite.setPosition(playerBox.getPosition());
		currentSprite = 6;
		attackSprite = -1;
		hp = 100;
	}

	void updateSpriteNumber(int i)
	{
		if (i == 0) //left
		{
			if ((currentSprite < 12 || currentSprite >= 18))
			{
				currentSprite = 12;
			}
		}
		else if (i == 1) //right
		{
			if ((currentSprite < 18 || currentSprite >= 24))
			{
				currentSprite = 18;
			}
		}
		else if (i == 2) //up
		{
			if (currentSprite >= 6)
			{
				currentSprite = 0;
			}
		}
		else if (i == 3) //down
		{
			if ((currentSprite < 6 || currentSprite >= 12))
			{
				currentSprite = 6;
			}
		}
		else if (i == 4) //attack
		{
			if (currentSprite < 6)
			{
				attackSprite = 48;
			}
			else if (currentSprite < 12)
			{
				attackSprite = 53;
			}
			else if (currentSprite < 18)
			{
				attackSprite = 58;
			}
			else if (currentSprite < 24)
			{
				attackSprite = 63;
			}
		}
	}

	void updateSprite()
	{
		if (attackSprite == -1)
		{
			IntRect playerSpriteRect = IntRect(32 * (currentSprite % 8), 40 * (int)(currentSprite / 8), 32, 40);
			if (currentSprite == 5)
			{
				currentSprite = 0;
			}
			else if (currentSprite == 11)
			{
				currentSprite = 6;
			}
			else if (currentSprite == 17)
			{
				currentSprite = 12;
			}
			else if (currentSprite == 23)
			{
				currentSprite = 18;
			}
			else
				currentSprite += 1;
			sprite.setTextureRect(playerSpriteRect);
		}
		else
		{
			IntRect playerSpriteRect = IntRect(32 * (attackSprite % 8), 40 * (int)(attackSprite / 8), 32, 40);
			if (attackSprite == 52 || attackSprite == 57 || attackSprite == 62 || attackSprite == 67)
			{
				attackSprite = -1;
			}
			else
			{
				attackSprite += 1;
			}
			sprite.setTextureRect(playerSpriteRect);
		}
	}

	Sprite* getSprite()
	{
		return &sprite;
	}
};