#include "Aya.h"

Aya::Aya(Vector2f p)
{
	aya = RectangleShape(Vector2f(32,40));
	aya.setPosition(p);
	aya.setFillColor(Color::Red);
	collisionBox = RectangleShape(Vector2f(64, 80));
	collisionBox.setOrigin(Vector2f(16, 20));
	collisionBox.setPosition(aya.getPosition());
	velocity = Vector2f(0, 0);
	counter = 4;
	moveState = 0;
}

Aya::~Aya()
{
}

std::shared_ptr<Projectile> Aya::shoot(Vector2f playerPosition)
{
	std::shared_ptr<Projectile> cproj(new CircleProjectile);
	cproj->spawn(aya.getPosition(), playerPosition);
	return cproj;
}

Shape* Aya::getEnemy()
{
	return &aya;
}

void Aya::setSprite(Texture &texture)
{
	IntRect enemySpriteRect(128, 0, 32, 40);
	sprite = Sprite(texture, enemySpriteRect);
	sprite.setPosition(aya.getPosition());
	currentSprite = 4;
	prevDirection = -1;
}

void Aya::updateSpriteNumber(int i)
{
	if (i != prevDirection)
	{
		if (i == 0) //left
		{
			if ((currentSprite < 8 || currentSprite >= 12))
			{
				currentSprite = 8;
			}
		}
		else if (i == 1) //right
		{
			if ((currentSprite < 12 || currentSprite >= 16))
			{
				currentSprite = 12;
			}
		}
		else if (i == 2) //up
		{
			if (currentSprite >= 4)
			{
				currentSprite = 0;
			}
		}
		else if (i == 3) //down
		{
			if ((currentSprite < 4 || currentSprite >= 8))
			{
				currentSprite = 4;
			}
		}
	}
	prevDirection = i;
}

void Aya::updateSprite()
{
	IntRect playerSpriteRect = IntRect(32 * (currentSprite % 8), 40 * (int)(currentSprite / 8), 32, 40);
	if (currentSprite == 3)
	{
		currentSprite = 0;
	}
	else if (currentSprite == 7)
	{
		currentSprite = 4;
	}
	else if (currentSprite == 11)
	{
		currentSprite = 8;
	}
	else if (currentSprite == 15)
	{
		currentSprite = 12;
	}
	else
		currentSprite += 1;
	sprite.setTextureRect(playerSpriteRect);
}

Sprite* Aya::getSprite()
{
	return &sprite;
}

void Aya::enemyPathfinder(std::vector<double> &mapMatrix, Vector2f g, std::vector<int> &workVector)
{
	Vector2f v = aya.getPosition();
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
		counter = 4;
		moveState = 0;
	}
	fill(workVector.begin(), workVector.end(), 0);
	return;
}

void Aya::updateEnemy(std::vector<std::shared_ptr<Enemy>> &enemyVector)
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

	aya.move(velocity);

	moveState = 0;
	for (const auto &e : enemyVector)
	{
		if (this == e.get())
			continue;
		if (e->getMoveState() == 1)
		{
		}
		else if (e->getMoveState() == 2)
		{
			if (aya.getGlobalBounds().intersects(e->getEnemy()->getGlobalBounds()))
			{
				moveState = 2;
				break;
			}
		}
		else if (aya.getGlobalBounds().intersects(e->getEnemy()->getGlobalBounds()))
		{
			moveState = 1;
			break;
		}
	}

	if (moveState == 0)
	{
		sprite.setPosition(aya.getPosition());
		collisionBox.move(velocity);
		++counter;
	}
	else
	{
		aya.move(-velocity);
	}
}

void Aya::clearStack()
{
	while (!path.empty())
		path.pop();
	moveState = 2;
}

RectangleShape* Aya::getCollisionBox()
{
	return &collisionBox;
}

int Aya::getMoveState()
{
	return moveState;
}