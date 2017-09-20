#include "Enemy.h"

Enemy::Enemy(Vector2f position) : 
	velocity(0,0), counter(4), moveState(0), currentAni(3), range(false), hp(100.f), character(Vector2f(32,40)), hpBar(Vector2f(32,5))
{
	//character = RectangleShape(Vector2f(32, 40));
	character.setPosition(position);

	//hpBar = RectangleShape(Vector2f(32, 5));
	hpBar.setPosition(position - Vector2f(0, 5));
	hpBar.setFillColor(Color::Red);
}

FloatRect Enemy::getBounds() const
{
	return character.getGlobalBounds();
}

const Sprite* Enemy::getSprite() const
{
	return animationVector[currentAni].getSprite();
}

void Enemy::targetReached()
{
	while (!path.empty())
		path.pop();
	if (range)
		moveState = 1;
	else
		moveState = 2;
}

int Enemy::getMoveState() const
{
	return moveState;
}

void Enemy::updateAnimation(const Player &player)
{
	animationVector[currentAni].update();
	if (currentAni >= 4 && animationVector[currentAni].isOver())
	{
		if (!range && character.getGlobalBounds().intersects(player.getBounds()))
		{
			player.takeDamage(5.f);
		}
		setCurrentAnimation(currentAni - 4);
	}
}

void Enemy::updateSpritePosition()
{
	animationVector[currentAni].setPosition(character.getPosition());
}

void Enemy::shoot(Vector2f v)
{

}

void Enemy::setCurrentAnimation(int i)
{
	if (i != currentAni)
	{
		animationVector[currentAni].resetFrame();
		currentAni = i;
	}
}

void Enemy::updateEnemy(const std::vector<std::unique_ptr<Enemy>> &enemyVector)
{
	if (path.empty())
		return;
	if (currentAni >= 4)
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
		setCurrentAnimation(direction);
		counter = 0;
	}

	character.move(velocity);

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
			if (getBounds().intersects(e->getBounds()))
			{
				moveState = 2;
				break;
			}
		}
		else if (getBounds().intersects(e->getBounds()))
		{
			moveState = 1;
			break;
		}
	}

	if (moveState == 0)
	{
		hpBar.move(velocity);
		animationVector[currentAni].move(velocity);
		++counter;
	}
	else
	{
		character.move(-velocity);
	}
}

void Enemy::enemyPathfinder(const std::vector<double> &mapMatrix, Vector2f g, std::vector<int> &workVector)
{
	Vector2f v = character.getPosition();
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
		counter = 4;
		moveState = 0;
	}
	fill(workVector.begin(), workVector.end(), 0);
	return;
}

void Enemy::setRangeAnimation(const Texture &t, float speed)
{

}

bool Enemy::ranged() const
{
	return range;
}

bool Enemy::inRange(Vector2f v) const
{
	return magnitude(v - character.getPosition()) < 300.f;
}

void Enemy::meleeAttack(Vector2f v)
{
	if (currentAni < 4)
	{
		v -= (character.getPosition() + Vector2f(16, 20));
		if (abs(v.y) > abs(v.x))
		{
			if (v.y < 0)
			{
				setCurrentAnimation(6);
			}
			else
			{
				setCurrentAnimation(7);
			}
		}
		else
		{
			if (v.x < 0)
			{
				setCurrentAnimation(4);
			}
			else
			{
				setCurrentAnimation(5);
			}
		}
	}
}

void Enemy::rangeAttack(Vector2f v)
{
	if (range && currentAni < 4)
	{
		moveState = 1;

		v -= (character.getPosition() + Vector2f(16, 20));
		if (abs(v.y) > abs(v.x))
		{
			if (v.y < 0)
			{
				setCurrentAnimation(6);
			}
			else
			{
				setCurrentAnimation(7);
			}
		}
		else
		{
			if (v.x < 0)
			{
				setCurrentAnimation(4);
			}
			else
			{
				setCurrentAnimation(5);
			}
		}
	}
}

void Enemy::updateProjectile()
{
	for (size_t i = 0; i < enemyProjectile.size(); ++i)
	{
		if (!enemyProjectile[i]->updateProjectile())
		{
			enemyProjectile.erase(enemyProjectile.begin() + i);
			--i;
		}
		else
		{
			enemyProjectile[i]->updateAnimation();
			enemyProjectile[i]->updateSpritePosition();
		}
	}
}

void Enemy::drawEnemy(RenderWindow &window) const
{
	window.draw(*animationVector[currentAni].getSprite());
	window.draw(hpBar);
}

void Enemy::drawProjectiles(RenderWindow &window) const
{
	for (const auto &p : enemyProjectile)
	{
		p->draw(window);
	}
}

void Enemy::calcProjCollision(const Player &player)
{
	for (size_t i = 0; i < enemyProjectile.size(); ++i)
	{
		float dealt = 0;
		dealt = enemyProjectile[i]->projDamageCalc(player.getBounds());
		if (dealt > 0)
		{
			player.takeDamage(dealt);
			if (enemyProjectile[i]->isOver())
			{
				enemyProjectile.erase(enemyProjectile.begin() + i);
				--i;
			}
		}
	}
}

void Enemy::takeDamage(float dmg) const
{
	hpBar.scale(Vector2f((hp - dmg) / hp, 1));
	hp -= dmg;
}

float Enemy::getCurrentHP() const
{
	return hp;
}