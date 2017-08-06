#include "SquareEnemy.h"
#include "AStar.h"

SquareEnemy::SquareEnemy()
{
}

SquareEnemy::~SquareEnemy()
{
}

void SquareEnemy::spawn(Vector2f p)
{
	sf::RectangleShape rectangle(p);
	rectangle.setSize(Vector2f(32,40));
	rectangle.setFillColor(Color::Red);
	enemy = rectangle;
	velocity = Vector2f(0,0);
	counter = 0;
}

std::shared_ptr<Projectile> SquareEnemy::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj (new CircleProjectile);
	cproj->spawn(enemy.getPosition(), playerPosition);
	//allProjectiles->push_back(cproj);
	return cproj;
}

Shape* SquareEnemy::getEnemy()
{
	return &enemy;
}

void SquareEnemy::setSprite(Texture &texture)
{
	IntRect enemySpriteRect(192, 0, 32, 40);
	sprite = Sprite(texture, enemySpriteRect);
	sprite.setPosition(enemy.getPosition());
	currentSprite = 6;
	prevDirection = -1;
}

void SquareEnemy::updateSpriteNumber(int i)
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

void SquareEnemy::updateSprite()
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

Sprite* SquareEnemy::getSprite()
{
	return &sprite;
}

void SquareEnemy::enemyPathfinder(Vector2f g, std::vector<int> &workVector)
{
	Vector2f v = enemy.getPosition();
	int start = (128 * (int)(v.y / 10)) + (int)(v.x / 10);
	int goal = (128 * (int)(g.y / 10)) + (int)(g.x / 10);
	//std::vector<int> result(128 * 72, 0);
	bool success = AStar(*mapMatrix, start, goal, workVector);

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

void SquareEnemy::updateEnemy()
{
	if (path.empty())
		return;
	if (counter % 4 == 0)
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
	}

	enemy.move(velocity);

	sprite.setPosition(enemy.getPosition());

	++counter;
}

void SquareEnemy::clearStack()
{
	while (!path.empty())
		path.pop();
}