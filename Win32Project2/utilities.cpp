#include "utilities.h"

float determinant(Vector2f v1, Vector2f v2)
{
	return v1.x * v2.y - v2.x * v1.y;
}

float dotProduct(Vector2f v1, Vector2f v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float magnitude(Vector2f v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

void updateMapMatrix(std::vector<double> &mapMatrix, Vector2f topLeft, Vector2f bottomRight, int value)
{
	for (int i = topLeft.y / 10; i < bottomRight.y / 10; ++i)
	{
		for (int j = topLeft.x / 10; j < bottomRight.x / 10; ++j)
		{
			mapMatrix[i * 128 + j] = value;
		}
	}
}

Vector2f rotateVector2f(Vector2f v, double angle)
{
	return Vector2f(v.x * cos(angle) + v.y * (-sin(angle)), v.x * sin(angle) + v.y * cos(angle));
}