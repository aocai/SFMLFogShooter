#include "Suika.h"

Suika::Suika()
{
}

Suika::~Suika()
{
}

void Suika::spawn(Vector2f p)
{
	RectangleShape shape = RectangleShape();
	suika = shape;
	suika.setPosition(p);
	suika.setSize(Vector2f(32, 40));
	suika.setFillColor(Color::Red);
	velocity = Vector2f(0, 0);
	counter = 4;
}

std::shared_ptr<Projectile> Suika::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj(new CircleProjectile);
	cproj->spawn(suika.getPosition(), playerPosition);
	return cproj;
}

Shape* Suika::getEnemy()
{
	return &suika;
}

void Suika::setSprite(Texture &texture)
{
	IntRect enemySpriteRect(128, 0, 32, 40);
	sprite = Sprite(texture, enemySpriteRect);
	sprite.setPosition(suika.getPosition());
	currentSprite = 8;
	prevDirection = -1;
}

void Suika::updateSpriteNumber(int i)
{
	if (i != prevDirection)
	{
		if (i == 0) //left
		{
			if ((currentSprite < 16 || currentSprite >= 24))
			{
				currentSprite = 16;
			}
		}
		else if (i == 1) //right
		{
			if ((currentSprite < 24 || currentSprite >= 32))
			{
				currentSprite = 24;
			}
		}
		else if (i == 2) //up
		{
			if (currentSprite >= 8)
			{
				currentSprite = 0;
			}
		}
		else if (i == 3) //down
		{
			if ((currentSprite < 8 || currentSprite >= 16))
			{
				currentSprite = 8;
			}
		}
	}
	prevDirection = i;
}

void Suika::updateSprite()
{
	IntRect playerSpriteRect = IntRect(32 * (currentSprite % 8), 40 * (int)(currentSprite / 8), 32, 40);
	if (currentSprite == 7)
	{
		currentSprite = 0;
	}
	else if (currentSprite == 15)
	{
		currentSprite = 8;
	}
	else if (currentSprite == 23)
	{
		currentSprite = 16;
	}
	else if (currentSprite == 31)
	{
		currentSprite = 24;
	}
	else
		currentSprite += 1;
	sprite.setTextureRect(playerSpriteRect);
}

Sprite* Suika::getSprite()
{
	return &sprite;
}

void Suika::enemyPathfinder(std::vector<double> &mapMatrix, Vector2f g, std::vector<int> &workVector)
{
	Vector2f v = suika.getPosition();
	int start = (128 * (int)(v.y / 10)) + (int)(v.x / 10);
	int goal = (128 * (int)(g.y / 10)) + (int)(g.x / 10);
	bool success = AStar(mapMatrix, start, goal, workVector);

	if (success)
	{
		while (!path.empty())
		{
			path.pop();
		}
		while (start != goal)
		{
			//path.push(goal);
			int temp = workVector[goal];
			if (temp == goal - 1)
			{
				path.push(1); //go right
			}
			else if (temp == goal + 1)
			{
				path.push(0); // go left
			}
			else if (temp > goal)
			{
				path.push(2); // go up
			}
			else
			{
				path.push(3); //go down
			}
			goal = temp;
		}
	}
	fill(workVector.begin(), workVector.end(), 0);
	return;
}

void Suika::updateEnemy()
{
	if (path.empty())
		return;
	if (counter == 4)
	{
		int direction = path.top();
		path.pop();
		Vector2f v;
		if (direction == 0)
		{
			v = Vector2f(-10, 0);
		}
		else if (direction == 1)
		{
			v = Vector2f(10, 0);
		}
		else if (direction == 2)
		{
			v = Vector2f(0, -10);
		}
		else
		{
			v = Vector2f(0, 10);
		}
		v /= 4.0f;
		velocity = v;
		updateSpriteNumber(direction);
		counter = 0;
	}

	suika.move(velocity);

	sprite.setPosition(suika.getPosition());

	++counter;
}

void Suika::clearStack()
{
	while (!path.empty())
		path.pop();
}