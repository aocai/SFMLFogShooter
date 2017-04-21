#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include "SquareEnemy.h"
#include "CircleProjectile.h"
#include "AStar.h"
#include "main.h"

using namespace sf;

RectangleShape playerBody;
CircleShape playerHead;
std::vector<std::shared_ptr<CircleProjectile>> *allCircleProjectiles = new std::vector<std::shared_ptr<CircleProjectile>>();
std::vector<double> *mapMatrix = new std::vector<double>(64*36, 1);

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
	for (int i = topLeft.y / 20; i < bottomRight.y / 20; ++i)
	{
		for (int j = topLeft.x / 20; j < bottomRight.x / 20; ++j)
		{
			(*mapMatrix)[i*64 + j] = value;
		}
	}
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

void enemyPathfinder(Vector2f v, Vector2f g, stack<int> &enemyPath)
{
	int start = (64 * (int)(v.y / 20)) + (int)(v.x / 20);
	int goal = (64 * (int)(g.y / 20)) + (int)(g.x / 20);
	std::vector<int> result(64*36, 0);
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

	//creater player shape
//	RectangleShape playerBody;
	playerBody.setSize(Vector2f(46,20));
	playerBody.setOrigin(Vector2f(23, 10));
	playerBody.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));
	playerBody.setFillColor(Color::Black);

//	CircleShape playerHead(15);
	playerHead.setRadius(15);
	playerHead.setOrigin(Vector2f(15, 15));
	playerHead.setPosition(Vector2f(playerBody.getPosition().x, playerBody.getPosition().y));
	playerHead.setFillColor(Color::Black);

	Vector2f lastMousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

	//spawn enemy (TESTING)
	SquareEnemy s;
	s.spawn(Vector2f(20, 20));
	stack<int> enemyPath;
	enemyPathfinder(s.re.getPosition(), playerHead.getPosition(), enemyPath);

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

		//Check for collision and prevent movement if collision detected
		//do for all directional keys
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			playerBody.move(Vector2f(-3.0f, 0));
			playerHead.move(Vector2f(-3.0f, 0));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				if (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
				{
					while (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
					{
						playerBody.move(Vector2f(1.0f, 0));
						playerHead.move(Vector2f(1.0f, 0));
					}
					break;
				}
			}
			if (playerHead.getPosition().x - playerHead.getRadius() < 0)
			{
				playerBody.move(Vector2f(- (playerHead.getPosition().x - playerHead.getRadius()), 0));
				playerHead.move(Vector2f(- (playerHead.getPosition().x - playerHead.getRadius()), 0));
			}
			if (playerBody.getRotation() != 90)
			{
				playerBody.rotate(-playerBody.getRotation());
				playerBody.rotate(90);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			playerBody.move(Vector2f(3.0f, 0));
			playerHead.move(Vector2f(3.0f, 0));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				if (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
				{
					while (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
					{
						playerBody.move(Vector2f(-1.0f, 0));
						playerHead.move(Vector2f(-1.0f, 0));
					}
					break;
				}
			}
			if (playerHead.getPosition().x + playerHead.getRadius() > 1280)
			{
				playerBody.move(Vector2f(1280 - (playerHead.getRadius() + playerHead.getPosition().x), 0));
				playerHead.move(Vector2f(1280 - (playerHead.getRadius() + playerHead.getPosition().x), 0));
			}
			if (playerBody.getRotation() != 90)
			{
				playerBody.rotate(-playerBody.getRotation());
				playerBody.rotate(90);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			playerBody.move(Vector2f(0, -3.0f));
			playerHead.move(Vector2f(0, -3.0f));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				if (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
				{
					while (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
					{
						playerBody.move(Vector2f(0, 1.0f));
						playerHead.move(Vector2f(0, 1.0f));
					}
					break;
				}
			}
			if (playerHead.getPosition().y - playerHead.getRadius() < 0)
			{
				playerBody.move(Vector2f(0, - (playerHead.getPosition().y - playerHead.getRadius())));
				playerHead.move(Vector2f(0, - (playerHead.getPosition().y - playerHead.getRadius())));
			}
			if (playerBody.getRotation() != 0)
			{
				playerBody.rotate(-playerBody.getRotation());
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			playerBody.move(Vector2f(0, 3.0f));
			playerHead.move(Vector2f(0, 3.0f));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				if (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
				{
					while (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
					{
						playerBody.move(Vector2f(0, -1.0f));
						playerHead.move(Vector2f(0, -1.0f));
					}
					break;
				}
			}
			if (playerHead.getPosition().y + playerHead.getRadius() > 720)
			{
				playerBody.move(Vector2f(0, 720 - (playerHead.getRadius() + playerHead.getPosition().y)));
				playerHead.move(Vector2f(0, 720 - (playerHead.getRadius() + playerHead.getPosition().y)));
			}
			if (playerBody.getRotation() != 0)
			{
				playerBody.rotate(-playerBody.getRotation());
			}
		}

		//current mouse coordinates
		Vector2f mouseCoord = window.mapPixelToCoords(Mouse::getPosition(window));

		//current player position
		Vector2f playerPosition = playerHead.getPosition();
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
				//mouse position too close to player position.
				//project mouse position onto surface of circle
				float angle = asinf((mouseCoord.y - playerPosition.y) / magnitude(PQ));
				Vector2f V = Vector2f(30 * cosf(angle), 30 * sinf(angle));
				lastMousePosition = Vector2f(playerPosition.x <= mouseCoord.x ? playerPosition.x + V.x : playerPosition.x - V.x, playerPosition.y + V.y);
				PQ = playerPosition - lastMousePosition;
			}
			else
			{
				lastMousePosition = mouseCoord;
			}
		}

		//get the left and right vectors of the player view area (left and right LOS arms) by rotating vector PQ by 15 degrees
		Vector2f rightArm = Vector2f(PQ.x * cos(15) + PQ.y * (-sin(15)), PQ.x * sin(15) + PQ.y * cos(15)) * 100.0f;
		Vector2f leftArm = Vector2f(PQ.x * cos(-15) + PQ.y * (-sin(-15)), PQ.x * sin(-15) + PQ.y * cos(-15)) * 100.0f;

		//get los ConvexShapes
		std::vector<ConvexShape> los = clipLOS(playerPosition, mouseCoord, leftArm, rightArm);

		//check shadow ConvexShapes
		std::vector<ConvexShape> shadow = checkShadow(walls, playerPosition);

		//draw background color
		window.clear(Color::White);

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

		//pathfinding
		Time elapsed = pathClock.getElapsedTime();
		if (elapsed.asSeconds() > 1)
		{
			//check if walls are moved and update mapMatrix accordingly
			for (int i = 0; i < dirtyWalls.size(); ++i)
			{
				if (dirtyWalls[i] == 1)
				{
					updateMapMatrix(walls[i].getPosition(), walls[i].getPosition() + walls[i].getSize(), 1);
				}
			}
			enemyPathfinder(s.re.getPosition(), playerHead.getPosition(), enemyPath);
			pathClock.restart();

			s.shoot(); //fire a projectile
		}

		Time elapsed_1 = clock.getElapsedTime();
		if (elapsed_1.asSeconds() > 0.1 && !enemyPath.empty())
		{
			int index = enemyPath.top();
			enemyPath.pop();
			int x = index % 64;
			int y = index / 64;
			s.position = Vector2f(20 * x, 20 * y);
			s.re.setPosition(s.position);
			clock.restart();
		}
		//draw enemy
		window.draw(s.re);

		//update projectile
		for (unsigned int i = 0; i < allCircleProjectiles->size(); ++i)
		{
			(*allCircleProjectiles)[i]->position += (*allCircleProjectiles)[i]->velocity;
			//check for bounds
			if ((*allCircleProjectiles)[i]->position.x < 0 || (*allCircleProjectiles)[i]->position.x > 1280 ||
				(*allCircleProjectiles)[i]->position.y < 0 || (*allCircleProjectiles)[i]->position.y > 720)
			{
				allCircleProjectiles->erase(allCircleProjectiles->begin() + i);
			}
			else
			{
				(*allCircleProjectiles)[i]->ci.move((*allCircleProjectiles)[i]->velocity);
			}
		}

		//draw projectiles
		for (unsigned int i = 0; i < allCircleProjectiles->size(); ++i)
		{
			window.draw((*allCircleProjectiles)[i]->ci);
		}

		//draw player
		window.draw(playerBody);
		window.draw(playerHead);

		window.display();
	}
	return 0;
}