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
	normal = player.getPosition() - position;
	sf::RectangleShape rectangle(position);
	rectangle.setSize(Vector2f(25,25));
	rectangle.setFillColor(Color::Red);
	re = rectangle;
}

void SquareEnemy::destroy()
{

}

void SquareEnemy::shoot()
{
	std::shared_ptr<CircleProjectile> cproj (new CircleProjectile);
	normal = player.getPosition() - position;
	cproj->spawn(position, normal);
	allCircleProjectiles->push_back(cproj);
}