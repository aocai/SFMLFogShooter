#include "SquareEnemy.h"
#include "CircleProjectile.h"
#include "main.h";

SquareEnemy::SquareEnemy()
{
}

SquareEnemy::~SquareEnemy()
{
}

void SquareEnemy::spawn(Vector2f p)
{
	position = p;
	normal = playerHead.getPosition() - position;
	sf::RectangleShape rectangle(position);
	rectangle.setSize(Vector2f(25,25));
	re = rectangle;
}

void SquareEnemy::destroy()
{

}

void SquareEnemy::shoot()
{
	std::shared_ptr<CircleProjectile> cproj (new CircleProjectile);
	normal = playerHead.getPosition() - position;
	cproj->spawn(position, normal);
	allCircleProjectiles->push_back(cproj);
}