#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include "SquareEnemy.h"
#include "CircleProjectile.h"
#include "Player.h"
#include "main.h"

using namespace sf;

//RectangleShape player;
std::vector<double> *mapMatrix = new std::vector<double>(128*72, 1);

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

void updateMapMatrix(Vector2f topLeft, Vector2f bottomRight, int value)
{
	for (int i = topLeft.y / 10; i < bottomRight.y / 10; ++i)
	{
		for (int j = topLeft.x / 10; j < bottomRight.x / 10; ++j)
		{
			(*mapMatrix)[i*128 + j] = value;
		}
	}
}

Vector2f rotateVector2f(Vector2f v, double angle)
{
	return Vector2f(v.x * cos(angle) + v.y * (-sin(angle)), v.x * sin(angle) + v.y * cos(angle));
}

//Description: Clip 2D ray against 4 bounding planes of the screen
//param: position = player position
//		 Q = player position + vector from player to point off screen
//		 rect = returned result of intersection in the right direction
//return: plane number the intersection was detected in
int boundClipTest(Vector2f position, Vector2f Q, Vector2f &ret)
{
	Vector2f PQ = Q - position;
	Vector2f normal = PQ.y == 0 ? Vector2f(PQ.y, PQ.x * -1) : Vector2f(PQ.y * -1, PQ.x);
	float num = dotProduct(position, normal);

	float result;
	int planeNum = -1;

	//Clip against 4 bounds of screen
	//0 = left plane
	//1 = right plane
	//2 = top plane
	//3 = bottom plane
	for (unsigned int boundary = 0; boundary < 4; ++boundary)
	{
		switch (boundary)
		{
		case 0:
			result = num / normal.y;
			if (result >= 0 && result <= 720)
			{
				//if intersection and Q lies in the same direction with regards to position
				if (dotProduct(Q - position, Vector2f(0, result) - position) >= 0)
				{
					ret = Vector2f(0, result);
					planeNum = 0;
				}
			}
			break;
		case 1:
			result = (num - normal.x * 1280) / normal.y;
			if (result >= 0 && result <= 720)
			{
				//if intersection and Q lies in the same direction with regards to position
				if (dotProduct(Q - position, Vector2f(1280, result) - position) >= 0)
				{
					ret = Vector2f(1280, result);
					planeNum = 1;
				}
			}
			break;
		case 2:
			result = num / normal.x;
			if (result >= 0 && result <= 1280)
			{
				//if intersection and Q lies in the same direction with regards to position
				if (dotProduct(Q - position, Vector2f(result, 0) - position) >= 0)
				{
					ret = Vector2f(result, 0);
					planeNum = 2;
				}
			}
			break;
		case 3:
			result = (num - normal.y * 720) / normal.x;
			if (result >= 0 && result <= 1280)
			{
				//if intersection and Q lies in the same direction with regards to position
				if (dotProduct(Q - position, Vector2f(result, 720) - position) >= 0)
				{
					ret = Vector2f(result, 720);
					planeNum = 3;
				}
			}
			break;
		default:
			planeNum = -1;
			break;
		}
		if (planeNum != -1)
			break;
	}

	return planeNum;
}

//Description: Clip the two vectors of the player's line of sight (left and right) against the 4 bounding planes of the screen
//Params:	position = player position
//			mousePosition = mouse position on screen
//			leftArm = left LOS vector
//			rightArm = right LOS vector
//return:	vector of ConvexShape that covers the area not in player's LOS
std::vector<ConvexShape> clipLOS(Vector2f position, Vector2f mousePosition, Vector2f leftArm, Vector2f rightArm)
{
	std::vector<ConvexShape> losVect;
	Color fogColor = Color(80, 80, 80, 255);
	ConvexShape los(4);
	los.setPoint(0, position);

	Vector2f result;
	int plane1 = boundClipTest(position, leftArm, result);

	Vector2f result2;
	int plane2 = boundClipTest(position, rightArm, result2);

	//clip according to which plane was intersected
	if (plane1 == 0)
	{
		if (plane2 == 0)
		{
			los.setPoint(1, Vector2f(position.x, 0));
			los.setPoint(2, Vector2f(0, 0));
			los.setPoint(3, result.y > result2.y ? result2 : result);
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, 720));
			los2.setPoint(2, Vector2f(0, 720));
			los2.setPoint(3, result.y > result2.y ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(position.x, 0));
			los3.setPoint(1, Vector2f(1280, 0));
			los3.setPoint(2, Vector2f(1280, 720));
			los3.setPoint(3, Vector2f(position.x, 720));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 1)
		{
			if (position.y > mousePosition.y)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(0, 720));
				los.setPoint(3, Vector2f(position.x, 720));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, result2);
				los2.setPoint(2, Vector2f(1280, 720));
				los2.setPoint(3, Vector2f(position.x, 720));
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
			else
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(0, 0));
				los.setPoint(3, Vector2f(position.x, 0));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, result2);
				los2.setPoint(2, Vector2f(1280, 0));
				los2.setPoint(3, Vector2f(position.x, 0));
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
		}
		else if (plane2 == 2)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(0, 720));
			los.setPoint(3, Vector2f(position.x, 720));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, 720));
			los2.setPoint(2, Vector2f(1280, 720));
			los2.setPoint(3, Vector2f(1280, position.y));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(1280, position.y));
			los3.setPoint(2, Vector2f(1280, 0));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 3)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(0, 0));
			los.setPoint(3, Vector2f(position.x, 0));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, 0));
			los2.setPoint(2, Vector2f(1280, 0));
			los2.setPoint(3, Vector2f(1280, position.y));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, result2);
			los3.setPoint(2, Vector2f(1280, 720));
			los3.setPoint(3, Vector2f(1280, position.y));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
	}
	else if (plane1 == 1)
	{
		if (plane2 == 0)
		{
			if (position.y > mousePosition.y)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(1280, 720));
				los.setPoint(3, Vector2f(position.x, 720));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, result2);
				los2.setPoint(2, Vector2f(0, 720));
				los2.setPoint(3, Vector2f(position.x, 720));
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
			else
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(1280, 0));
				los.setPoint(3, Vector2f(position.x, 0));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, result2);
				los2.setPoint(2, Vector2f(0, 0));
				los2.setPoint(3, Vector2f(position.x, 0));
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
		}
		else if (plane2 == 1)
		{
			los.setPoint(1, Vector2f(position.x, 0));
			los.setPoint(2, Vector2f(1280, 0));
			los.setPoint(3, result.y > result2.y ? result2 : result);
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, 720));
			los2.setPoint(2, Vector2f(1280, 720));
			los2.setPoint(3, result.y > result2.y ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(position.x, 0));
			los3.setPoint(1, Vector2f(0, 0));
			los3.setPoint(2, Vector2f(0, 720));
			los3.setPoint(3, Vector2f(position.x, 720));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 2)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(1280, 720));
			los.setPoint(3, Vector2f(position.x, 720));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, 720));
			los2.setPoint(2, Vector2f(0, 720));
			los2.setPoint(3, Vector2f(0, position.y));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(0, position.y));
			los3.setPoint(2, Vector2f(0, 0));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 3)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(1280, 0));
			los.setPoint(3, Vector2f(position.x, 0));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, 0));
			los2.setPoint(2, Vector2f(0, 0));
			los2.setPoint(3, Vector2f(0, position.y));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(0, position.y));
			los3.setPoint(2, Vector2f(0, 720));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
	}
	else if (plane1 == 2)
	{
		if (plane2 == 0)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(1280, 0));
			los.setPoint(3, Vector2f(1280, position.y));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(1280, position.y));
			los2.setPoint(2, Vector2f(1280, 720));
			los2.setPoint(3, Vector2f(position.x, 720));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(position.x, 720));
			los3.setPoint(2, Vector2f(0, 720));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 1)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(0, 0));
			los.setPoint(3, Vector2f(0, position.y));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(0, position.y));
			los2.setPoint(2, Vector2f(0, 720));
			los2.setPoint(3, Vector2f(position.x, 720));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(position.x, 720));
			los3.setPoint(2, Vector2f(1280, 720));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 2)
		{
			los.setPoint(1, Vector2f(0, position.y));
			los.setPoint(2, Vector2f(0, 0));
			los.setPoint(3, result.x > result2.x ? result2 : result);
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(1280, position.y));
			los2.setPoint(2, Vector2f(1280, 0));
			los2.setPoint(3, result.x > result2.x ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(0, position.y));
			los3.setPoint(1, Vector2f(0, 720));
			los3.setPoint(2, Vector2f(1280, 720));
			los3.setPoint(3, Vector2f(1280, position.y));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 3)
		{
			if (position.x > mousePosition.x)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(1280, 0));
				los.setPoint(3, Vector2f(1280, position.y));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, Vector2f(1280, position.y));
				los2.setPoint(2, Vector2f(1280, 720));
				los2.setPoint(3, result2);
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
			else
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(0, 0));
				los.setPoint(3, Vector2f(0, position.y));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, Vector2f(0, position.y));
				los2.setPoint(2, Vector2f(0, 720));
				los2.setPoint(3, result2);
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
		}
	}
	else if (plane1 == 3)
	{
		if (plane2 == 0)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(1280, 720));
			los.setPoint(3, Vector2f(1280, position.y));			
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(1280, position.y));
			los2.setPoint(2, Vector2f(1280, 0));
			los2.setPoint(3, Vector2f(position.x, 0));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(position.x, 0));
			los3.setPoint(2, Vector2f(0, 0));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 1)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(0, 720));
			los.setPoint(3, Vector2f(0, position.y));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(0, position.y));
			los2.setPoint(2, Vector2f(0, 0));
			los2.setPoint(3, Vector2f(position.x, 0));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(position.x, 0));
			los3.setPoint(2, Vector2f(1280, 0));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 2)
		{
			if (position.x > mousePosition.x)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(1280, 720));
				los.setPoint(3, Vector2f(1280, position.y));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, Vector2f(1280, position.y));
				los2.setPoint(2, Vector2f(1280, 0));
				los2.setPoint(3, result2);
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
			else
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(0, 720));
				los.setPoint(3, Vector2f(0, position.y));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, Vector2f(0, position.y));
				los2.setPoint(2, Vector2f(0, 0));
				los2.setPoint(3, result2);
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
		}
		else if (plane2 == 3)
		{
			los.setPoint(1, Vector2f(0, position.y));
			los.setPoint(2, Vector2f(0, 720));
			los.setPoint(3, result.x > result2.x ? result2 : result);
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(1280, position.y));
			los2.setPoint(2, Vector2f(1280, 720));
			los2.setPoint(3, result.x > result2.x ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(0, 0));
			los3.setPoint(1, Vector2f(0, position.y));
			los3.setPoint(2, Vector2f(1280, position.y));
			los3.setPoint(3, Vector2f(1280, 0));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
	}

	return losVect;
}

//Description: Create wall shadow and clip against the 4 bounding planes of the screen
//Params:	wall = the current RectangleShape
//			position = player position
//return:	ConvexShape of the wall's shadow, clipped against the 4 bounding planes of the screen
ConvexShape clipShadow(RectangleShape wall, Vector2f position)
{
	int maxPoint = 0;
	int minPoint = 0;

	//for each point in RectangleShape
	for (unsigned int j = 0; j < 4; ++j)
	{
		int result = -1;

		//for each other point in RectangleShape, check the determinant of vectors from position to the two points.
		//if for a point, they are all greater than 0, then it is the maxPoint. If for a point, they are all lesser than 0, then it is the minPoint
		for (unsigned int k = 0; k < 4; ++k)
		{
			//do not process the same point twice
			if (k == j)
			{
				++k;
			}
			else
			{
				if (determinant(wall.getPosition() + wall.getPoint(j) - position, wall.getPosition() + wall.getPoint(k) - position) > 0)
				{
					if (result == 0)
					{
						result = -1;
						break;
					}
					result = 1;
				}
				else
				{
					if (result == 1)
					{
						result = -1;
						break;
					}
					result = 0;
				}
			}
		}
		if (result != -1)
		{
			if (result == 1)
				maxPoint = j;
			else if (result == 0)
				minPoint = j;
		}
	}

	//the maxPoint and minPoint are two points of the shadow ConvexShape
	ConvexShape shape(4);
	shape.setPoint(0, wall.getPosition() + wall.getPoint(maxPoint));
	shape.setPoint(1, wall.getPosition() + wall.getPoint(minPoint));

	//get the other points by projecting onto the boundaries of the screen, and clip against the 4 bounds
	Vector2f result;
	Vector2f u1 = position;
	Vector2f u2 = position + (wall.getPosition() + wall.getPoint(minPoint) - position) * 100.0f;
	int successful = boundClipTest(u1, u2, result);

	Vector2f result2;
	Vector2f v1 = position;
	Vector2f v2 = position + (wall.getPosition() + wall.getPoint(maxPoint) - position) * 100.0f;
	int successful2 = boundClipTest(v1, v2, result2);

	if (successful != successful2)
	{
		if (successful == 0)
		{
			if (successful2 == 2)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(0, 0));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 3)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(0, 720));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 1)
			{
				shape.setPointCount(6);
				if (position.y > wall.getPosition().y)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, 0));
					shape.setPoint(4, Vector2f(1280, 0));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, 720));
					shape.setPoint(4, Vector2f(1280, 720));
					shape.setPoint(5, result2);
				}
			}
		}
		else if (successful == 1)
		{
			if (successful2 == 2)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(1280, 0));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 3)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(1280, 720));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 0)
			{
				shape.setPointCount(6);
				if (position.y > wall.getPosition().y)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(1280, 0));
					shape.setPoint(4, Vector2f(0, 0));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(1280, 720));
					shape.setPoint(4, Vector2f(0, 720));
					shape.setPoint(5, result2);
				}
			}
		}
		else if (successful == 2)
		{
			if (successful2 == 0)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(0, 0));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 1)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(1280, 0));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 3)
			{
				shape.setPointCount(6);
				if (position.x > wall.getPosition().x)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, 0));
					shape.setPoint(4, Vector2f(0, 720));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(1280, 0));
					shape.setPoint(4, Vector2f(1280, 720));
					shape.setPoint(5, result2);
				}
			}
		}
		else if (successful == 3)
		{
			if (successful2 == 0)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(0, 720));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 1)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(1280, 720));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 2)
			{
				shape.setPointCount(6);
				if (position.x > wall.getPosition().x)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, 720));
					shape.setPoint(4, Vector2f(0, 0));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(1280, 720));
					shape.setPoint(4, Vector2f(1280, 0));
					shape.setPoint(5, result2);
				}
			}
		}
	}
	else
	{
		shape.setPoint(2, result);
		shape.setPoint(3, result2);
	}

	shape.setFillColor(Color(150, 150, 150, 255));
	return shape;
}

//clip fog/shadow by testing for intersection with left and right arms of LOS. Then clip against the boundaries of the view screen
std::vector<ConvexShape> checkShadow(std::vector<RectangleShape> walls, Vector2f position)
{
	std::vector<ConvexShape> fog;

	//for each wall, create the shadow according to player position
	for (unsigned int i = 0; i < walls.size(); ++i)
	{
		ConvexShape shape = clipShadow(walls[i], position);
		fog.push_back(shape);
	}
	return fog;
}

/*
void enemyPathfinder(Vector2f v, Vector2f g, stack<int> &enemyPath)
{
	int start = (128 * (int)(v.y / 10)) + (int)(v.x / 10);
	int goal = (128 * (int)(g.y / 10)) + (int)(g.x / 10);
	std::vector<int> result(128*72, 0);
	bool success = AStar(*mapMatrix, start, goal, result);

	if (success)
	{
		while (!enemyPath.empty())
		{
			enemyPath.pop();
		}
		while (start != goal)
		{
			enemyPath.push(goal);
			goal = result[goal];
		}
	}
	return;
}
*/

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;

	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(windowWidth, windowHeight), "FogShooter", Style::Default, settings);
	window.setFramerateLimit(60);

	Clock clock;
	Clock pathClock;
	Clock spriteClock;
	Clock attackClock;
	Clock CirnoClock;

	std::vector<RectangleShape> walls;

	//create the walls
	RectangleShape rect1;
	rect1.setSize(Vector2f(80,220));
	rect1.setPosition(Vector2f(120,110));
	rect1.setFillColor(Color::Black);
	walls.push_back(rect1);
	updateMapMatrix(rect1.getPosition(), rect1.getPosition() + rect1.getSize(), -1);

	RectangleShape rect2;
	rect2.setSize(Vector2f(80, 300));
	rect2.setPosition(Vector2f(520, 250));
	rect2.setFillColor(Color::Black);
	walls.push_back(rect2);
	updateMapMatrix(rect2.getPosition(), rect2.getPosition() + rect2.getSize(), -1);

	RectangleShape rect3;
	rect3.setSize(Vector2f(60, 160));
	rect3.setPosition(Vector2f(300, 50));
	rect3.setFillColor(Color::Black);
	walls.push_back(rect3);
	updateMapMatrix(rect3.getPosition(), rect3.getPosition() + rect3.getSize(), -1);

	RectangleShape rect4;
	rect4.setSize(Vector2f(250, 110));
	rect4.setPosition(Vector2f(750, 600));
	rect4.setFillColor(Color::Black);
	walls.push_back(rect4);
	updateMapMatrix(rect4.getPosition(), rect4.getPosition() + rect4.getSize(), -1);

	RectangleShape rect5;
	rect5.setSize(Vector2f(220, 90));
	rect5.setPosition(Vector2f(1000, 320));
	rect5.setFillColor(Color::Black);
	walls.push_back(rect5);
	updateMapMatrix(rect5.getPosition(), rect5.getPosition() + rect5.getSize(), -1);


	std::vector<std::shared_ptr<Projectile>> allProjectiles;

	Player player(Vector2f(32,40), Vector2f(windowWidth / 2, windowHeight / 2));

	Vector2f lastMousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

	std::vector<std::shared_ptr<Enemy>> enemyVector;

	//spawn enemy (TESTING)
	std::shared_ptr<Enemy> s(new SquareEnemy);
	s->spawn(Vector2f(20, 20));
	Texture Cirno;
	Cirno.loadFromFile("Cirno.png");
	s->setSprite(Cirno);

	enemyVector.push_back(s);

	//stack<int> enemyPath;
	std::vector<int> workVector(128 * 72, 0);
	s->enemyPathfinder(player.getPosition(), workVector);
	//enemyPathfinder(s.getEnemy()->getPosition(), player.getPosition(), enemyPath);
	Vector2f CirnoTarget = s->getEnemy()->getPosition();

	Texture Flandre;
	Flandre.loadFromFile("Flandre Scarlet.png");
	player.setSprite(Flandre);
	
	std::vector<int> dirtyWalls(walls.size(), 0);
	unsigned int currentWall = walls.size();
	float deltaX;
	float deltaY;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		//allow walls to be moved around by dragging with MLB
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (currentWall == walls.size())
			{
				for (unsigned int i = 0; i < walls.size(); ++i)
				{
					if (walls[i].getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window))))
					{
						//get wall thats clicked on
						currentWall = i;

						deltaX = window.mapPixelToCoords(Mouse::getPosition(window)).x - walls[currentWall].getPosition().x;
						deltaY = window.mapPixelToCoords(Mouse::getPosition(window)).y - walls[currentWall].getPosition().y;
						break;
					}
				}
			}
		}
		else
		{
			currentWall = walls.size();
			deltaX = 0;
			deltaY = 0;
		}

		//if wall is selected by MLB
		if (currentWall < walls.size())
		{
			Vector2f wallSize = walls[currentWall].getSize();

			//free current wall in mapMatrix if not already done so
			if (dirtyWalls[currentWall] == 0)
			{
				updateMapMatrix(walls[currentWall].getPosition(), walls[currentWall].getPosition() + wallSize, 1);
			}
			
			//move wall to new mouse position
			float posX = window.mapPixelToCoords(Mouse::getPosition(window)).x - deltaX;
			float posY = window.mapPixelToCoords(Mouse::getPosition(window)).y - deltaY;

			if (posX < 0)
				posX = 0;
			else if (posX + wallSize.x > 1280)
				posX = 1280 - wallSize.x;
			if (posY < 0)
				posY = 0;
			else if (posY + wallSize.y > 720)
				posY = 720 - wallSize.y;

			walls[currentWall].setPosition(Vector2f(posX, posY));

			//set Wall to dirty and to be updated in pathfinding
			dirtyWalls[currentWall] = 1;
		}

		//Player Movement. Prevent movement if collision detected
		//do for all directional keys
		if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
		{
			player.getPlayer()->move(Vector2f(-3.0f, 0));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				while (walls[i].getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
				{
					player.getPlayer()->move(Vector2f(1.0f, 0));
				}
			}
			if (player.getPosition().x < 0)
			{
				player.getPlayer()->move(Vector2f(-player.getPosition().x, 0));
			}

			player.updateSpriteNumber(0);
			player.getSprite()->setPosition(player.getPosition());
		}
		if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
		{
			player.getPlayer()->move(Vector2f(3.0f, 0));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				while (walls[i].getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
				{
					player.getPlayer()->move(Vector2f(-1.0f, 0));
				}
			}
			if (player.getPosition().x + player.getPlayer()->getSize().x > 1280)
			{
				player.getPlayer()->move(Vector2f(1280 - (player.getPlayer()->getSize().x + player.getPlayer()->getPosition().x), 0));
			}

			player.updateSpriteNumber(1);
			player.getSprite()->setPosition(player.getPosition());
		}
		if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
		{
			player.getPlayer()->move(Vector2f(0, -3.0f));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				while (walls[i].getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
				{
					player.getPlayer()->move(Vector2f(0, 1.0f));
				}
			}
			if (player.getPosition().y < 0)
			{
				player.getPlayer()->move(Vector2f(0, -player.getPlayer()->getPosition().y));
			}

			if (!(Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) && !(Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)))
			{
				player.updateSpriteNumber(2);
			}
			player.getSprite()->setPosition(player.getPosition());
		}
		if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
		{
			player.getPlayer()->move(Vector2f(0, 3.0f));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				while (walls[i].getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
				{
					player.getPlayer()->move(Vector2f(0, -1.0f));
				}
			}
			if (player.getPlayer()->getPosition().y + player.getPlayer()->getSize().y > 720)
			{
				player.getPlayer()->move(Vector2f(0, 720 - (player.getPlayer()->getSize().y + player.getPlayer()->getPosition().y)));
			}

			if (!(Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) && !(Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)))
			{
				player.updateSpriteNumber(3);
			}
			player.getSprite()->setPosition(player.getPosition());
		}

		//check for attack input
		if (Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Numpad0))
		{
			player.updateSpriteNumber(4);
		}

		//set player move sprites
		if (spriteClock.getElapsedTime().asSeconds() > 0.10)
		{
			player.updateSprite();
			spriteClock.restart();
		}

		//current mouse coordinates
		Vector2f mouseCoord = window.mapPixelToCoords(Mouse::getPosition(window));

		//current player position
		Vector2f playerPosition = player.getPosition() + Vector2f(16, 20);
		Vector2f PQ;

		//get vector from player position to mouse position
		if (playerPosition == mouseCoord)
		{
			PQ = playerPosition - lastMousePosition;
		}
		else
		{
			PQ = playerPosition - mouseCoord;
			if (magnitude(PQ) < 30)
			{
				//mouse position too close to player position. project mouse position onto surface of circle
				PQ = (PQ / magnitude(PQ)) * 30.f;
			}
			lastMousePosition = mouseCoord;
		}

		//get the left and right vectors of the player view area (left and right LOS arms) by rotating vector PQ by 15 degrees

		Vector2f rightArm = rotateVector2f(PQ, 15) * 100.0f;
		Vector2f leftArm = rotateVector2f(PQ, -15) * 100.0f;

		//get los ConvexShapes
		std::vector<ConvexShape> los = clipLOS(playerPosition, mouseCoord, leftArm, rightArm);

		//check shadow ConvexShapes
		std::vector<ConvexShape> shadow = checkShadow(walls, playerPosition);

		//draw background color
		window.clear(Color::White);

		//pathfinding
		Time elapsed = pathClock.getElapsedTime();
		if (elapsed.asSeconds() > 1)
		{
			//check if walls are moved and update mapMatrix accordingly
			for (int i = 0; i < dirtyWalls.size(); ++i)
			{
				if (dirtyWalls[i] == 1)
				{
					updateMapMatrix(walls[i].getPosition(), walls[i].getPosition() + walls[i].getSize(), -1);
					dirtyWalls[i] = 0;
				}
			}
			for (int i = 0; i < enemyVector.size(); ++i)
			{
				if (!enemyVector[i]->getEnemy()->getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
				{
					enemyVector[i]->enemyPathfinder(player.getPosition(), workVector);
				}
			}
			//s->enemyPathfinder(player.getPosition(), workVector);
			//enemyPathfinder(s.getEnemy()->getPosition(), player.getPosition(), enemyPath);
			pathClock.restart();

			//TODO: fire projectile only if in range and no obstacles
			allProjectiles.push_back(s->shoot(playerPosition)); //fire a projectile
		}

		//do everything
		for (int i = 0; i < enemyVector.size(); ++i)
		{
			enemyVector[i]->updateEnemy();
			if (enemyVector[i]->getEnemy()->getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
			{
				//reached
				enemyVector[i]->clearStack();
			}
		}

		/*
		if (magnitude(CirnoTarget - s->getEnemy()->getPosition()) < 3)
		{
			if (!s->path.empty())
			{
				int index = s->path.top();
				s->path.pop();
				int x = index % 128;
				int y = index / 128;
				CirnoTarget = Vector2f(10 * x, 10 * y);
			}
		}

		if (magnitude(s->getEnemy()->getPosition() - player.getPosition()) > 5 && !s->path.empty())
		{
			//interpolate Cirno to target position
			Vector2f unitV = CirnoTarget - s->getEnemy()->getPosition();
			unitV = unitV / magnitude(unitV);
			unitV *= 3.f;

			int awsd = 0;
			float maxf = 0;
			if (abs(unitV.x) > maxf)
			{
				maxf = abs(unitV.x);
				awsd = unitV.x < 0 ? 0 : 1;
			}
			if (abs(unitV.y) > maxf)
			{
				maxf = abs(unitV.y);
				awsd = unitV.y < 0 ? 2 : 3;
			}

			s->getEnemy()->move(unitV);

			while (s->getEnemy()->getGlobalBounds().intersects(player.getPlayer()->getGlobalBounds()))
			{
				s->getEnemy()->move(unitV / -3.f);
			}

			s->getSprite()->setPosition(s->getEnemy()->getPosition());

			s->updateSpriteNumber(awsd);
		}
		else
		{
			while (!s->path.empty())
				s->path.pop();
		}
		*/
		
		Time elapsed_1 = CirnoClock.getElapsedTime();
		if (elapsed_1.asSeconds() > 0.1)
		{
			s->updateSprite();
			CirnoClock.restart();
		}

		/*
		//move enemy
		Time elapsed_1 = clock.getElapsedTime();
		if (elapsed_1.asSeconds() > 0.1 && !enemyPath.empty())
		{
			int index = enemyPath.top();
			enemyPath.pop();
			int x = index % 128;
			int y = index / 128;
			Vector2f newPosition = Vector2f(10 * x, 10 * y);
			s.getEnemy()->setPosition(newPosition);
			clock.restart();
		}
		*/

		//draw enemy
		window.draw(*(s->getSprite()));

		//update projectile
		for (unsigned int i = 0; i < allProjectiles.size(); ++i)
		{
			allProjectiles[i]->position += allProjectiles[i]->velocity;
			//check for bounds
			if (allProjectiles[i]->position.x < 0 || allProjectiles[i]->position.x > 1280 ||
				allProjectiles[i]->position.y < 0 || allProjectiles[i]->position.y > 720)
			{
				allProjectiles.erase(allProjectiles.begin() + i);
			}
			else
			{
				allProjectiles[i]->getProjectile()->move(allProjectiles[i]->velocity);
			}
		}

		//draw projectiles
		for (unsigned int i = 0; i < allProjectiles.size(); ++i)
		{
			window.draw(*(allProjectiles[i]->getProjectile()));
		}

		//draw shadow first
		for (unsigned int i = 0; i < shadow.size(); ++i)
		{
			window.draw(shadow[i]);
		}

		//draw player view area by drawing the opposite
		for (unsigned int i = 0; i < los.size(); ++i)
		{
			window.draw(los[i]);
		}

		//draw walls
		for (unsigned int i = 0; i < walls.size(); ++i)
		{
			window.draw(walls[i]);
		}

		//draw player
		window.draw(*(player.getSprite()));

		window.display();
	}
	return 0;
}