#include "Cirno.h"

Cirno::Cirno()
{
}

Cirno::~Cirno()
{
}

void Cirno::spawn(Vector2f p)
{
	RectangleShape shape = RectangleShape();
	cirno = shape;
	cirno.setPosition(p);
	cirno.setSize(Vector2f(32,40));
	cirno.setFillColor(Color::Red);
	velocity = Vector2f(0,0);
	counter = 4;
}

std::shared_ptr<Projectile> Cirno::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj (new CircleProjectile);
	cproj->spawn(cirno.getPosition(), playerPosition);
	return cproj;
}

Shape* Cirno::getEnemy()
{
	return &cirno;
}

void Cirno::setSprite(Texture &texture)
{
	IntRect enemySpriteRect(192, 0, 32, 40);
	sprite = Sprite(texture, enemySpriteRect);
	sprite.setPosition(cirno.getPosition());
	currentSprite = 6;
	prevDirection = -1;
}

void Cirno::updateSpriteNumber(int i)
{
	if (i != prevDirection)
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
	}
	prevDirection = i;
}

void Cirno::updateSprite()
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

Sprite* Cirno::getSprite()
{
	return &sprite;
}

void Cirno::enemyPathfinder(std::vector<double> &mapMatrix, Vector2f g, std::vector<int> &workVector)
{
	Vector2f v = cirno.getPosition();
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

void Cirno::updateEnemy()
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

	cirno.move(velocity);

	sprite.setPosition(cirno.getPosition());

	++counter;
}

void Cirno::clearStack()
{
	while (!path.empty())
		path.pop();
}