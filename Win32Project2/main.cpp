#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <winsock2.h>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <thread>
#include <iostream>
#include "main.h"
#include "Server.h"

using namespace sf;

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
	for (int boundary = 0; boundary < 4; ++boundary)
	{
		switch (boundary)
		{
		case 0:
			result = num / normal.y;
			if (result >= 0 && result <= windowHeight)
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
			result = (num - normal.x * windowWidth) / normal.y;
			if (result >= 0 && result <= windowHeight)
			{
				//if intersection and Q lies in the same direction with regards to position
				if (dotProduct(Q - position, Vector2f(windowWidth, result) - position) >= 0)
				{
					ret = Vector2f(windowWidth, result);
					planeNum = 1;
				}
			}
			break;
		case 2:
			result = num / normal.x;
			if (result >= 0 && result <= windowWidth)
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
			result = (num - normal.y * windowHeight) / normal.x;
			if (result >= 0 && result <= windowWidth)
			{
				//if intersection and Q lies in the same direction with regards to position
				if (dotProduct(Q - position, Vector2f(result, windowHeight) - position) >= 0)
				{
					ret = Vector2f(result, windowHeight);
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
			los2.setPoint(1, Vector2f(position.x, windowHeight));
			los2.setPoint(2, Vector2f(0, windowHeight));
			los2.setPoint(3, result.y > result2.y ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(position.x, 0));
			los3.setPoint(1, Vector2f(windowWidth, 0));
			los3.setPoint(2, Vector2f(windowWidth, windowHeight));
			los3.setPoint(3, Vector2f(position.x, windowHeight));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 1)
		{
			if (position.y > mousePosition.y)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(0, windowHeight));
				los.setPoint(3, Vector2f(position.x, windowHeight));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, result2);
				los2.setPoint(2, Vector2f(windowWidth, windowHeight));
				los2.setPoint(3, Vector2f(position.x, windowHeight));
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
				los2.setPoint(2, Vector2f(windowWidth, 0));
				los2.setPoint(3, Vector2f(position.x, 0));
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
		}
		else if (plane2 == 2)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(0, windowHeight));
			los.setPoint(3, Vector2f(position.x, windowHeight));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, windowHeight));
			los2.setPoint(2, Vector2f(windowWidth, windowHeight));
			los2.setPoint(3, Vector2f(windowWidth, position.y));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(windowWidth, position.y));
			los3.setPoint(2, Vector2f(windowWidth, 0));
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
			los2.setPoint(2, Vector2f(windowWidth, 0));
			los2.setPoint(3, Vector2f(windowWidth, position.y));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, result2);
			los3.setPoint(2, Vector2f(windowWidth, windowHeight));
			los3.setPoint(3, Vector2f(windowWidth, position.y));
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
				los.setPoint(2, Vector2f(windowWidth, windowHeight));
				los.setPoint(3, Vector2f(position.x, windowHeight));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, result2);
				los2.setPoint(2, Vector2f(0, windowHeight));
				los2.setPoint(3, Vector2f(position.x, windowHeight));
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
			else
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(windowWidth, 0));
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
			los.setPoint(2, Vector2f(windowWidth, 0));
			los.setPoint(3, result.y > result2.y ? result2 : result);
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, windowHeight));
			los2.setPoint(2, Vector2f(windowWidth, windowHeight));
			los2.setPoint(3, result.y > result2.y ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(position.x, 0));
			los3.setPoint(1, Vector2f(0, 0));
			los3.setPoint(2, Vector2f(0, windowHeight));
			los3.setPoint(3, Vector2f(position.x, windowHeight));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 2)
		{
			los.setPoint(1, result);
			los.setPoint(2, Vector2f(windowWidth, windowHeight));
			los.setPoint(3, Vector2f(position.x, windowHeight));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(position.x, windowHeight));
			los2.setPoint(2, Vector2f(0, windowHeight));
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
			los.setPoint(2, Vector2f(windowWidth, 0));
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
			los3.setPoint(2, Vector2f(0, windowHeight));
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
			los.setPoint(2, Vector2f(windowWidth, 0));
			los.setPoint(3, Vector2f(windowWidth, position.y));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(windowWidth, position.y));
			los2.setPoint(2, Vector2f(windowWidth, windowHeight));
			los2.setPoint(3, Vector2f(position.x, windowHeight));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(position.x, windowHeight));
			los3.setPoint(2, Vector2f(0, windowHeight));
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
			los2.setPoint(2, Vector2f(0, windowHeight));
			los2.setPoint(3, Vector2f(position.x, windowHeight));
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, position);
			los3.setPoint(1, Vector2f(position.x, windowHeight));
			los3.setPoint(2, Vector2f(windowWidth, windowHeight));
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
			los2.setPoint(1, Vector2f(windowWidth, position.y));
			los2.setPoint(2, Vector2f(windowWidth, 0));
			los2.setPoint(3, result.x > result2.x ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(0, position.y));
			los3.setPoint(1, Vector2f(0, windowHeight));
			los3.setPoint(2, Vector2f(windowWidth, windowHeight));
			los3.setPoint(3, Vector2f(windowWidth, position.y));
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 3)
		{
			if (position.x > mousePosition.x)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(windowWidth, 0));
				los.setPoint(3, Vector2f(windowWidth, position.y));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, Vector2f(windowWidth, position.y));
				los2.setPoint(2, Vector2f(windowWidth, windowHeight));
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
				los2.setPoint(2, Vector2f(0, windowHeight));
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
			los.setPoint(2, Vector2f(windowWidth, windowHeight));
			los.setPoint(3, Vector2f(windowWidth, position.y));
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(windowWidth, position.y));
			los2.setPoint(2, Vector2f(windowWidth, 0));
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
			los.setPoint(2, Vector2f(0, windowHeight));
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
			los3.setPoint(2, Vector2f(windowWidth, 0));
			los3.setPoint(3, result2);
			los3.setFillColor(fogColor);
			losVect.push_back(los3);
		}
		else if (plane2 == 2)
		{
			if (position.x > mousePosition.x)
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(windowWidth, windowHeight));
				los.setPoint(3, Vector2f(windowWidth, position.y));
				los.setFillColor(fogColor);
				losVect.push_back(los);

				ConvexShape los2(4);
				los2.setPoint(0, position);
				los2.setPoint(1, Vector2f(windowWidth, position.y));
				los2.setPoint(2, Vector2f(windowWidth, 0));
				los2.setPoint(3, result2);
				los2.setFillColor(fogColor);
				losVect.push_back(los2);
			}
			else
			{
				los.setPoint(1, result);
				los.setPoint(2, Vector2f(0, windowHeight));
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
			los.setPoint(2, Vector2f(0, windowHeight));
			los.setPoint(3, result.x > result2.x ? result2 : result);
			los.setFillColor(fogColor);
			losVect.push_back(los);

			ConvexShape los2(4);
			los2.setPoint(0, position);
			los2.setPoint(1, Vector2f(windowWidth, position.y));
			los2.setPoint(2, Vector2f(windowWidth, windowHeight));
			los2.setPoint(3, result.x > result2.x ? result : result2);
			los2.setFillColor(fogColor);
			losVect.push_back(los2);

			ConvexShape los3(4);
			los3.setPoint(0, Vector2f(0, 0));
			los3.setPoint(1, Vector2f(0, position.y));
			los3.setPoint(2, Vector2f(windowWidth, position.y));
			los3.setPoint(3, Vector2f(windowWidth, 0));
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
	for (int j = 0; j < 4; ++j)
	{
		int result = -1;

		//for each other point in RectangleShape, check the determinant of vectors from position to the two points.
		//if for a point, they are all greater than 0, then it is the maxPoint. If for a point, they are all lesser than 0, then it is the minPoint
		for (int k = 0; k < 4; ++k)
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
				shape.setPoint(3, Vector2f(0, windowHeight));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 1)
			{
				shape.setPointCount(6);
				if (position.y > wall.getPosition().y)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, 0));
					shape.setPoint(4, Vector2f(windowWidth, 0));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, windowHeight));
					shape.setPoint(4, Vector2f(windowWidth, windowHeight));
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
				shape.setPoint(3, Vector2f(windowWidth, 0));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 3)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(windowWidth, windowHeight));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 0)
			{
				shape.setPointCount(6);
				if (position.y > wall.getPosition().y)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(windowWidth, 0));
					shape.setPoint(4, Vector2f(0, 0));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(windowWidth, windowHeight));
					shape.setPoint(4, Vector2f(0, windowHeight));
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
				shape.setPoint(3, Vector2f(windowWidth, 0));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 3)
			{
				shape.setPointCount(6);
				if (position.x > wall.getPosition().x)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, 0));
					shape.setPoint(4, Vector2f(0, windowHeight));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(windowWidth, 0));
					shape.setPoint(4, Vector2f(windowWidth, windowHeight));
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
				shape.setPoint(3, Vector2f(0, windowHeight));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 1)
			{
				shape.setPointCount(5);
				shape.setPoint(2, result);
				shape.setPoint(3, Vector2f(windowWidth, windowHeight));
				shape.setPoint(4, result2);
			}
			else if (successful2 == 2)
			{
				shape.setPointCount(6);
				if (position.x > wall.getPosition().x)
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(0, windowHeight));
					shape.setPoint(4, Vector2f(0, 0));
					shape.setPoint(5, result2);
				}
				else
				{
					shape.setPoint(2, result);
					shape.setPoint(3, Vector2f(windowWidth, windowHeight));
					shape.setPoint(4, Vector2f(windowWidth, 0));
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
	for (int i = 0; i < walls.size(); ++i)
	{
		ConvexShape shape = clipShadow(walls[i], position);
		fog.push_back(shape);
	}
	return fog;
}


const unsigned short PORT = 5000;
const std::string ipAddress = "174.6.144.24";
//std::vector<std::unique_ptr<sf::TcpSocket>> sockets;
sf::SocketSelector clientSocketSelector;
sf::TcpSocket TcpSocketToServer;
sf::UdpSocket UdpSendSocket;
sf::UdpSocket UdpReceiveSocket;
char UdpSendBuffer[508];
//sf::TcpSocket tcpsocket;
bool connected = false;
bool tryConnect = true;
bool tryListen = true;
bool roomReady = false;
bool opponentLeft = false;
bool gameReady = false;
int myPlayerNumber = -1;
std::string hostAddress;
std::vector<bool> playersConnected(4, false);
std::vector<bool> playersReadied(4, false);
Player player{ Vector2f(32,40), Vector2f(0,0) };
std::vector<Player> players;
std::unordered_map<int, std::unique_ptr<Projectile>> projMap;
Animation straightProjAni;
Animation SpiralProjAni;
Animation ExpandProjAni;

int counter = 0;
int counter2 = 0;
std::queue<std::string> packetQueue;
std::queue <std::string> serverPacketQueue;
std::vector<RectangleShape> customWalls;
sf::Font font;
sf::Music BGM;
sf::Texture brickTexture;

/*
class ReuseableListener : public sf::TcpListener
{
public:
	ReuseableListener()
	{
		create();
		reuse();
	}
	void reuse()
	{
		char reuse = 1;
		setsockopt(getHandle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	}
};

void connectionListen()
{
	std::cout << "connectionListen thread start here" << std::endl;
	static ReuseableListener listener;
	std::cout << "accepting socket on port " << PORT << std::endl;
	listener.close();
	listener.listen(PORT);

	sf::SocketSelector selector;
	selector.add(listener);
	while (tryListen)
	{
		if (selector.wait(sf::milliseconds(2000)))
		{
			if (selector.isReady(listener))
			{
				auto socket = std::make_unique<sf::TcpSocket>();
				if (listener.accept(*socket) == sf::Socket::Done)
				{
					std::cout << "socket accepted on ip " << socket->getRemoteAddress().toString() << std::endl;
					sockets.emplace_back(std::move(socket));
					clientSocketSelector.add(*sockets.back());

					int i = 0;
					for (; i < playersConnected.size() && playersConnected[i]; ++i);
					std::string s{ "PLAYER_CONNECTED" };
					s += ";" + to_string(i);
				}
			}
		}
	}
	std::cout << "connectionListen thread ends here" << std::endl;
}

void processServerPackets()
{
	while (!serverPacketQueue.empty())
	{
		string s = serverPacketQueue.front();
		serverPacketQueue.pop();
		string code = "";
		auto i = s.begin();
		for (; i != s.end() && *i != ';'; ++i)
		{
			code += *i;
		}
		std::cout << code << std::endl;
		if (code == "PLAYER_CONNECTED")
		{
			std::cout << "server received packet PLAYER_CONNECTED!" << std::endl;
			i++;
			int playerNum = *i - '0';
			if (playersConnected[playerNum])
			{
				std::cout << "error, player already connected!" << std::endl;
			}
			else
			{
				playersConnected[playerNum] = true;
				std::cout << "player " << playerNum << " has connected!" << std::endl;
			}
		}
		else if (code == "PLAYER_DISCONNECTED")
		{
			std::cout << "server received packet PLAYER_DISCONNECTED!" << std::endl;
			i++;
			int playerNum = *i - '0';
			if (!playersConnected[playerNum])
			{
				std::cout << "error, player already disconnected!" << std::endl;
			}
			else
			{
				playersConnected[playerNum] = false;
				std::cout << "player " << playerNum << " has disconnected!" << std::endl;
			}
		}
		else if (code == "PLAYER_READY")
		{
			std::cout << "server received packet PLAYER_READY!" << std::endl;
			i++;
			int playerNum = *i - '0';
			playersReadied[playerNum] = true;
			std::cout << "player " << playerNum << " has readied up!" << std::endl;
		}
		else if (code == "PLAYER_UNREADY")
		{

		}
		else if (code == "PLAYER_MOVE")
		{
			i++;
			string x = "";
			string y = "";
			for (; i != s.end() && *i != ','; ++i)
			{
				x += *i;
			}
			i++;
			for (; i != s.end(); ++i)
			{
				y += *i;
			}
			Vector2f newMovement{ static_cast<float>(atof(x.c_str())), static_cast<float>(atof(y.c_str())) };
			p.move(newMovement, customWalls);
		}
		else if (code == "PROJECTILE_NORMAL")
		{
			i++;
			string x = "";
			string y = "";
			for (; i != s.end() && *i != ','; ++i)
			{
				x += *i;
			}
			i++;
			for (; i != s.end(); ++i)
			{
				y += *i;
			}
			p.rangeAttack(Vector2f{ static_cast<float>(atof(x.c_str())), static_cast<float>(atof(y.c_str())) });
		}
		else if (code == "PROJECTILE_SPIRAL")
		{
			p.shootSpiral();
		}
		else if (code == "PROJECTILE_EXPAND")
		{
			i++;
			string x = "";
			string y = "";
			for (; i != s.end() && *i != ','; ++i)
			{
				x += *i;
			}
			i++;
			for (; i != s.end(); ++i)
			{
				y += *i;
			}
			p.shootExpand(Vector2f{ static_cast<float>(atof(x.c_str())), static_cast<float>(atof(y.c_str())) });
		}
	}
}

void server()
{
	std::cout << "server thread start here" << std::endl;
	std::cout << "server running... IP is " << sf::IpAddress::getPublicAddress().toString() << std::endl;
	std::thread connectionListenThread(&connectionListen);
	std::thread receiveThread(&receiveMessageFromClients);
	
	processServerPackets();

	connectionListenThread.join();
	std::cout << "server thread ends here" << std::endl;
}
*/

void UdpClient()
{
	sf::Clock clock;
	sf::Time time;
	char *receiveBuffer = static_cast<char*>(malloc(1500));
	while (connected)
	{
		time = clock.getElapsedTime();
		std::size_t received;
		sf::IpAddress sender;
		unsigned short port;
		UdpReceiveSocket.receive(receiveBuffer, 508, received, sender, port);

		if (received <= 0)
			continue;

		std::cout << "Client on port " << UdpReceiveSocket.getLocalPort() << " received " << received << " bytes from " << sender.toString() << " on port " << port << std::endl;

		UdpSnapshot snap;
		snapshotInit(&snap, receiveBuffer, received);
		//process message
		int GameSnapCount = snapshotReadInt(&snap);
		std::cout << "GameSnapCount = " << GameSnapCount << std::endl;

		short PlayerCount = snapshotReadShort(&snap);
		std::cout << "PlayerCount = " << PlayerCount << std::endl;

		std::vector<int> validProjectileIDs;

		for (short i = 0; i < PlayerCount; ++i)
		{
			short PlayerID = snapshotReadShort(&snap);
			std::cout << "PlayerID = " << PlayerID << std::endl;
			Vector2f PlayerPos{ snapshotReadFloat(&snap), snapshotReadFloat(&snap) };
			std::cout << "PlayerPos = " << PlayerPos.x << "," << PlayerPos.y << std::endl;
			float hp = snapshotReadFloat(&snap);
			std::cout << "hp = " << hp << std::endl;

			if (PlayerID == myPlayerNumber)
			{
				//TODO check past snapshot for position accuracy

				player.takeDamage(player.getCurrentHP() - hp);
			}
			else
			{
				for (auto &p : players)
				{
					if (p.getID() == PlayerID)
					{
						Vector2f diff = PlayerPos - p.getPosition();
						//TODO: interpolate movement
						if (magnitude(diff) > 0)
							p.move(diff, customWalls);

						p.takeDamage(p.getCurrentHP() - hp);
						break;
					}
				}
			}

			short ProjectileCount = snapshotReadShort(&snap);

			int playerIndex = [&]() -> int {
				for (int i = 0; i < players.size(); ++i)
				{
					if (players[i].getID() == PlayerID)
						return i;
				}
				return -1;
			}();

			//auto projectileVector = playerIndex == -1 ? player.getProjectiles() : players[playerIndex].getProjectiles();

			std::cout << "ProjectileCount = " << ProjectileCount << std::endl;

			for (short j = 0; j < ProjectileCount; ++j)
			{
				short ProjectileID = snapshotReadShort(&snap);
				std::cout << "ProjectileID = " << ProjectileID << std::endl;

				validProjectileIDs.emplace_back(ProjectileID);

				short ProjectileType = snapshotReadShort(&snap);
				std::cout << "ProjectileType = " << ProjectileType << std::endl;
				Vector2f ProjectilePos{ snapshotReadFloat(&snap), snapshotReadFloat(&snap) };
				std::cout << "ProjectilePos = " << ProjectilePos.x << "," << ProjectilePos.y << std::endl;
				//short DataLength = snapshotReadShort(&snap);
				//std::cout << "DataLength = " << DataLength << std::endl;
				Vector2f target;
				long long mask;
				if (ProjectileType == 0)
				{
					target = { snapshotReadFloat(&snap), snapshotReadFloat(&snap) };
				}
				else if (ProjectileType == 1)
				{
					mask = snapshotReadLongLong(&snap);
				}
				else if (ProjectileType == 2)
				{
					target = { snapshotReadFloat(&snap), snapshotReadFloat(&snap) };
				}

				if (projMap.find(ProjectileID) == projMap.end())
				{
					if (ProjectileType == 0)
					{
						projMap[ProjectileID] = std::make_unique<StraightProjectile>(PlayerPos + Vector2f(16, 20), target, Vector2f(14, 40));
						Vector2f v = target - (PlayerPos + Vector2f(16, 20));
						Vector2f w(0, -1);
						float angle = atan2(determinant(v, w), dotProduct(v, w)) * 180.0 / 3.14159265;
						projMap[ProjectileID]->setAnimation(straightProjAni, -angle);
					}
					else if (ProjectileType == 1)
					{
						projMap[ProjectileID] = std::make_unique<SpiralProjectile>(PlayerPos + Vector2f(16, 20), 8.f);
						projMap[ProjectileID]->setAnimation(SpiralProjAni, 0);
					}
					else if (ProjectileType == 2)
					{
						projMap[ProjectileID] = std::make_unique<ExpandProjectile>(PlayerPos + Vector2f(16, 20), target, 5.f, 30.f);
						projMap[ProjectileID]->setAnimation(SpiralProjAni, 0);
					}
				}
				else
				{
					if (ProjectileType == 1)
					{
						dynamic_cast<SpiralProjectile*>(projMap[ProjectileID].get())->setNotValidMask(mask);
						std::cout << "set mask " << mask << std::endl;
					}
				}

				/*
				int projIndex = 0;
				for (; projIndex < projectileVector->size(); ++projIndex)
				{
					std::cout << (*projectileVector)[projIndex]->getID() << std::endl;
					std::cout << ProjectileID << std::endl;
					if ((*projectileVector)[projIndex]->getID() == ProjectileID)
					{
						if (ProjectileType == 1) //if spiral
						{
							std::cout << "clientside setting notvalidmask to " << mask << std::endl;
							dynamic_cast<SpiralProjectile*>((*projectileVector)[projIndex].get())->setNotValidMask(mask);
						}
						break;
					}
				}

				//doesnt exist in player vector
				if (projIndex >= projectileVector->size())
				{
					if (ProjectileType == 0)
					{
						if (playerIndex == -1)
							player.rangeAttack(target);
						else
							players[playerIndex].rangeAttack(target);
					}
					else if (ProjectileType == 1)
					{
						if (playerIndex == -1)
							player.shootSpiral();
						else
							players[playerIndex].shootSpiral();
						std::cout << "ran once" << std::endl;
					}
					else if (ProjectileType == 2)
					{
						if (playerIndex == -1)
							player.shootExpand(target);
						else
							players[playerIndex].shootExpand(target);
					}
				}
				*/
			}
		}
		for (auto it = projMap.begin(); it != projMap.end(); )
		{
			if ([&]()->bool {
				for (const auto &id : validProjectileIDs)
				{
					if (it->first == id)
						return false;
				}
				return true;
			}())
			{
				it = projMap.erase(it);
			}
			else
			{
				++it;
			}
		}
		sf::sleep(sf::microseconds(1000000 / 60) - time);
	}
	std::free(receiveBuffer);
	std::cout << "UdpClientThread ends here" << std::endl;
}

void client(std::string ipAddr)
{
	std::cout << "client thread start here" << std::endl;
	if (ipAddr == sf::IpAddress::getPublicAddress().toString())
	{
		std::cout << "Attempting connection to own IP not allowed. Client thread end here" << std::endl;
		std::cout << "Ignored for now" << std::endl;
		//return;
	}

	sf::IpAddress ip;
	if (ipAddr.empty())
		ip = sf::IpAddress();
	else
		ip = sf::IpAddress(ipAddr);
	std::cout << "Attemping conenction on ip " << ip.toString() << std::endl;

	//try connect with incremental timeout upto 2 seconds
	auto timeout = sf::milliseconds(200);

	auto status = TcpSocketToServer.connect(ip, PORT, timeout);
	while (tryConnect && status != sf::Socket::Done)
	{
		std::cout << "Could not connect. Status code: " << status << ". Trying again with timeout = " << timeout.asMilliseconds() << "..." << std::endl;
		timeout = sf::milliseconds(std::min(2000, 2 * timeout.asMilliseconds()));
		status = TcpSocketToServer.connect(ip, PORT, timeout);
	}
	if (status == sf::Socket::Done)
	{
		std::cout << "Connected on ip " << TcpSocketToServer.getRemoteAddress() << std::endl;
		connected = true;
	}

	std::thread UdpClientThread(&UdpClient);
	while (connected)
	{
		sf::Packet packet;
		auto status = TcpSocketToServer.receive(packet);
		if (status == sf::Socket::Done)
		{
			if (!packet.endOfPacket())
			{
				string s;
				packet >> s;
				packetQueue.push(s);
			}
		}
		else if (status == sf::Socket::Disconnected)
		{
			connected = false;
			break;
		}
	}

	UdpClientThread.join();
	std::cout << "Client thread ends here" << std::endl;
}

/*
void receiveMessageFromClients()
{
	std::cout << "receiveMessageFromClients thread start here" << std::endl;
	while (true)
	{
		if (clientSocketSelector.wait(sf::milliseconds(2000)))
		{
			for (const auto &socket : sockets)
			{
				if (clientSocketSelector.isReady(*socket))
				{
					sf::Packet packet;
					auto status = socket->receive(packet);
					if (status == sf::Socket::Done)
					{
						string s;
						packet >> s;
						serverPacketQueue.push(s);
					}
				}
			}
		}
	}
	std::cout << "receivemsg thread ends here" << std::endl;
}

void receiveMessageFromServer()
{
	std::cout << "receiveMessageFromServer thread start here" << std::endl;
	while (true)
	{
		sf::Packet packet;
		//auto status = tcpsocket.receive(packet);
		auto status = TcpSocketToServer.receive(packet);
		if (status == sf::Socket::Disconnected)
		{
			connected = false;
			break;
		}
		if (!packet.endOfPacket())
		{
			string s;
			packet >> s;
			packetQueue.push(s);
		}
	}
	std::cout << "receiveMessageFromServer thread ends here" << std::endl;
}
*/

void sendSocketMessageToServer(std::string message)
{
	sf::Packet packet;
	packet << message;
	
	while (TcpSocketToServer.send(packet) == sf::Socket::Partial);
	++counter2;
	std::cout << "sent " << counter2 << " packets" << std::endl;
}

void sendPlayerInputToServer(PlayerFrameInput &input)
{
	UdpSnapshot snap;
	memset(&UdpSendBuffer, 0, 508);
	snapshotInit(&snap, UdpSendBuffer, 508);
	snapshotWriteChar(&snap, input.playerID);
	snapshotWriteFloat(&snap, input.deltaX);
	snapshotWriteFloat(&snap, input.deltaY);
	snapshotWriteInt(&snap, input.SkillUsedMask);
	if (input.SkillUsedMask & 5)
	{
		snapshotWriteFloat(&snap, input.targetX);
		snapshotWriteFloat(&snap, input.targetY);
	}
	UdpSendSocket.send(snap.data, snap.currentSize, "174.6.144.24", 5001);
}

/*
void sendSocketMessageToClients(std::string message)
{
	sf::Packet packet;
	packet << message;

	for (const auto &socket : sockets)
	{
		while (socket->send(packet) == sf::Socket::Partial);
		++counter2;
		std::cout << "sent " << counter2 << " packets" << std::endl;
	}
}
*/

void processInitPacketQueue()
{
	while (!packetQueue.empty())
	{
		string s = packetQueue.front();
		packetQueue.pop();
		std::cout << s << std::endl;
		string code = "";
		auto it = s.begin();
		for (; it != s.end() && *it != ';'; ++it)
		{
			code += *it;
		}
		if (code == "ROOM_READY")
		{
			std::cout << "received packet ROOM_READY!" << std::endl;
			roomReady = true;
		}
		else if (code == "CONNECTED_AS")
		{
			std::cout << "received packet CONNECTED_AS!" << std::endl;
			++it;
			myPlayerNumber = static_cast<int>(*it - '0');
			std::cout << "connected as player " << myPlayerNumber << std::endl;
			Player p{ Vector2f(32,40), Vector2f(myPlayerNumber * 200.f, windowHeight / 2.f), myPlayerNumber };
			//players.emplace_back(std::move(p));
			player = std::move(p);
		}
		else if (code == "PLAYER_CONNECTED")
		{
			std::cout << "received packet PLAYER_CONNECTED!" << std::endl;
			++it;
			int playerNum = static_cast<int>(*it - '0');
			if (playersConnected[playerNum - 1])
			{
				std::cout << "error, player already connected!" << std::endl;
			}
			else
			{
				playersConnected[playerNum - 1] = true;
				std::cout << "player " << playerNum << " has connected!" << std::endl;
				//Player p{ Vector2f(32,40), Vector2f(playerNum * 200.f, windowHeight / 2.f), playerNum };
				//players.emplace_back(std::move(p));
				players.emplace_back(Vector2f(32, 40), Vector2f(playerNum * 200.f, windowHeight / 2.f), playerNum);
			}
		}
		else if (code == "PLAYER_DISCONNECTED")
		{
			std::cout << "received packet PLAYER_DISCONNECTED!" << std::endl;
			++it;
			int playerNum = static_cast<int>(*it - '0');
			if (!playersConnected[playerNum - 1])
			{
				std::cout << "error, player already disconnected!" << std::endl;
			}
			else
			{
				playersConnected[playerNum - 1] = false;
				std::cout << "player " << playerNum << " has disconnected!" << std::endl;
				for (int i = 0; i < players.size(); ++i)
				{
					if (players[i].getID() == playerNum)
					{
						players.erase(players.begin() + i);
						break;
					}
				}
			}
		}
		else if (code == "PLAYER_READY")
		{
			std::cout << "received packet PLAYER_READY!" << std::endl;
			++it;
			int playerNum = static_cast<int>(*it - '0');
			if (playersReadied[playerNum - 1])
			{
				std::cout << "error, player already readied!" << std::endl;
			}
			else
			{
				playersReadied[playerNum] = true;
				std::cout << "player " << playerNum << " has readied up!" << std::endl;
			}
		}
		else if (code == "PLAYER_UNREADY")
		{
			std::cout << "received packet PLAYER_READY!" << std::endl;
			++it;
			int playerNum = static_cast<int>(*it - '0');
			if (!playersReadied[playerNum])
			{
				std::cout << "error, player already not ready!" << std::endl;
			}
			else
			{
				playersReadied[playerNum] = false;
				std::cout << "player " << playerNum << " has unreadied!" << std::endl;
			}
		}
		else if (code == "ALL_PLAYERS_READY")
		{
			std::cout << "received packet ALL_PLAYERS_READY!" << std::endl;
			gameReady = true;
		}
		else if (code == "CREATE_WALL")
		{
			std::cout << "received packet CREATE_WALL!" << std::endl;

			++it;
			std::string upperLeftX;
			std::string upperLeftY;
			std::string sizeX;
			std::string sizeY;
			for (; it != s.end() && *it != ','; ++it)
			{
				upperLeftX += *it;
			}
			++it;
			for (; it != s.end() && *it != ';'; ++it)
			{
				upperLeftY += *it;
			}
			++it;
			for (; it != s.end() && *it != ','; ++it)
			{
				sizeX += *it;
			}
			++it;
			for (; it != s.end() && *it != ';'; ++it)
			{
				sizeY += *it;
			}
			RectangleShape r{ Vector2f{ static_cast<float>(atof(sizeX.c_str())), static_cast<float>(atof(sizeY.c_str())) } };
			r.setPosition(Vector2f{ static_cast<float>(atof(upperLeftX.c_str())), static_cast<float>(atof(upperLeftY.c_str())) });
			r.setTexture(&brickTexture);
			r.setTextureRect({ 0,0,10 * static_cast<int>(r.getSize().x),10 * static_cast<int>(r.getSize().y) });
			customWalls.emplace_back(r);
		}
		else if (code == "DELETE_WALL")
		{
			std::cout << "received packet DELETE_WALL!" << std::endl;
			++it;

			int wallIndex = 0;
			while (it != s.end())
			{
				wallIndex = wallIndex * 10 + static_cast<int>(*it - '0');
				++it;
			}
			customWalls.erase(customWalls.begin() + wallIndex);
		}
	}
}

/*
void processPlayerPacketQueue(Player &p)
{
	while (!packetQueue.empty())
	{
		string s = packetQueue.front();
		packetQueue.pop();
		string code = "";
		auto i = s.begin();
		for (; i != s.end() && *i != ';'; ++i)
		{
			code += *i;
		}
		std::cout << code << std::endl;
		if (code == "PLAYER_MOVE")
		{
			i++;
			string x = "";
			string y = "";
			for (; i != s.end() && *i != ','; ++i)
			{
				x += *i;
			}
			i++;
			for (; i != s.end(); ++i)
			{
				y += *i;
			}
			Vector2f newMovement{ static_cast<float>(atof(x.c_str())), static_cast<float>(atof(y.c_str())) };
			p.move(newMovement, customWalls);
		}
		else if (code == "PROJECTILE_NORMAL")
		{
			i++;
			string x = "";
			string y = "";
			for (; i != s.end() && *i != ','; ++i)
			{
				x += *i;
			}
			i++;
			for (; i != s.end(); ++i)
			{
				y += *i;
			}
			p.rangeAttack(Vector2f{ static_cast<float>(atof(x.c_str())), static_cast<float>(atof(y.c_str())) });
		}
		else if (code == "PROJECTILE_SPIRAL")
		{
			p.shootSpiral();
		}
		else if (code == "PROJECTILE_EXPAND")
		{
			i++;
			string x = "";
			string y = "";
			for (; i != s.end() && *i != ','; ++i)
			{
				x += *i;
			}
			i++;
			for (; i != s.end(); ++i)
			{
				y += *i;
			}
			p.shootExpand(Vector2f{ static_cast<float>(atof(x.c_str())), static_cast<float>(atof(y.c_str())) });
		}
		else if (code == "PLAYER_DISCONNECT")
		{
			std::cout << "received disconnect" << std::endl;
			opponentLeft = true;
		}
		++counter;
		std::cout << "received " << counter << " packets" << std::endl;
	}
}
*/

//return false to exit to lobby, 1 to restart on same map
void GameOverScreen(RenderWindow &window, std::string str)
{
	//tcpsocket.disconnect();

	sf::Music gameOverBgm;
	gameOverBgm.openFromFile("Resources\\Music\\Bloom_Nobly.ogg");
	gameOverBgm.setVolume(10);
	gameOverBgm.setLoop(true);
	gameOverBgm.play();

	sf::Text gameOverText;
	gameOverText.setString("Game Over!");
	gameOverText.setColor(Color::Black);
	gameOverText.setFont(font);
	gameOverText.setOrigin(Vector2f(gameOverText.getGlobalBounds().width / 2.f, gameOverText.getGlobalBounds().height / 2.f));
	gameOverText.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 3.f));

	sf::Text resultText;
	resultText.setString(str);
	resultText.setColor(Color::Black);
	resultText.setFont(font);
	resultText.setOrigin(Vector2f(resultText.getGlobalBounds().width / 2.f, resultText.getGlobalBounds().height / 2.f));
	resultText.setPosition(gameOverText.getPosition() + Vector2f(0, 50));

	sf::Text returnText;
	returnText.setString("Return");
	returnText.setColor(Color::Black);
	returnText.setFont(font);
	returnText.setOrigin(Vector2f(returnText.getGlobalBounds().width / 2.f, returnText.getGlobalBounds().height / 2.f));
	returnText.setPosition(resultText.getPosition() + Vector2f(0,50));

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
					if (returnText.getGlobalBounds().contains(mousePos))
					{
						//tcpsocket.disconnect();
						//sockets[0]->disconnect();
						std::vector<RectangleShape>().swap(customWalls);
						gameOverBgm.stop();
						return;
					}
				}
			}
		}
		window.clear(Color::White);
		window.draw(gameOverText);
		window.draw(resultText);
		window.draw(returnText);
		window.display();
	}
	std::vector<RectangleShape>().swap(customWalls);
	gameOverBgm.stop();
	return;
}

void createPlayArea(RenderWindow &window)
{
	sf::Text hintText;
	hintText.setString("Click and drag to create walls, and press Enter to confirm!");
	hintText.setColor(Color::Black);
	hintText.setFont(font);
	hintText.setOrigin({ hintText.getGlobalBounds().width / 2.f, hintText.getGlobalBounds().height / 2.f });
	hintText.setPosition({ windowWidth / 2.f,  50.f });

	Vector2f mouseDown;

	RectangleShape r;
	r.setFillColor(sf::Color::Transparent);
	r.setOutlineColor(sf::Color::Black);
	r.setOutlineThickness(5.f);
	bool valid = false;

	bool pause = false;
	//char *receiveBuffer = static_cast<char*>(malloc(1500));

	while (window.isOpen() && !roomReady)
	{
		PlayerFrameInput input = { myPlayerNumber, 0.f, 0.f, 0 };
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				TcpSocketToServer.disconnect();
				window.close();
			}
			if (event.type == Event::LostFocus)
				pause = true;
			if (event.type == Event::GainedFocus)
				pause = false;
			if (valid && event.type == sf::Event::MouseMoved)
			{
				r.setSize({ std::abs(mouseDown.x - static_cast<float>(event.mouseMove.x)), std::abs(mouseDown.y - static_cast<float>(event.mouseMove.y)) });
				r.setPosition({ std::min(mouseDown.x, static_cast<float>(event.mouseMove.x)), std::min(mouseDown.y, static_cast<float>(event.mouseMove.y)) });
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					mouseDown = { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };
					r.setPosition(mouseDown);
					r.setSize({ 0,0 });
					valid = true;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					Vector2f v{ static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };
					for (int i = 0; i < customWalls.size(); ++i)
					{
						if (customWalls[i].getGlobalBounds().contains(v))
						{
							std::string s{ "DELETE_WALL" };
							s += ";" + to_string(i);
							sendSocketMessageToServer(s);
							std::cout << "sending delete on wallIndex = " << i << std::endl;
							break;
						}
					}
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (r.getSize().x >= 5 && r.getSize().y >= 5)
					{
						std::string s{ "CREATE_WALL" };
						s += ";" + to_string(r.getPosition().x) + "," + to_string(r.getPosition().y) + ";" + to_string(r.getSize().x) + "," + to_string(r.getSize().y);
						sendSocketMessageToServer(s);
					}
					valid = false;
				}
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape ||
					event.key.code == sf::Keyboard::Return)
				{
					std::string s{ "ROOM_READY" };
					s += ";" + to_string(myPlayerNumber);
					sendSocketMessageToServer(s);
					//return;
				}
			}
		}
		/*
		//receive server snapshot
		while (true)
		{
			std::size_t received;
			sf::IpAddress sender;
			unsigned short port;
			UdpReceiveSocket.receive(receiveBuffer, 508, received, sender, port);

			if (received <= 0)
				break;

			std::cout << "Client on port " << UdpReceiveSocket.getLocalPort() <<" received " << received << " bytes from " << sender.toString() << " on port " << port << std::endl;

			UdpSnapshot snap;
			snapshotInit(&snap, receiveBuffer, received);
			//process message
			int GameSnapCount = snapshotReadInt(&snap);
			std::cout << "GameSnapCount = " << GameSnapCount << std::endl;

			short PlayerCount = snapshotReadShort(&snap);
			std::cout << "PlayerCount = " << PlayerCount << std::endl;
			for (short i = 0; i < PlayerCount; ++i)
			{
				short PlayerID = snapshotReadShort(&snap);
				std::cout << "PlayerID = " << PlayerID << std::endl;
				Vector2f PlayerPos{ snapshotReadFloat(&snap), snapshotReadFloat(&snap) };
				std::cout << "PlayerPos = " << PlayerPos.x << "," << PlayerPos.y << std::endl;

				if (PlayerID == myPlayerNumber)
				{
					//TODO check past snapshot for position accuracy
				}
				else
				{
					for (auto &p : players)
					{
						if (p.getPlayerID() == PlayerID)
						{
							Vector2f diff = PlayerPos - p.getPosition();
							if (magnitude(diff) > 0)
								p.move(diff, customWalls);
							break;
						}
					}
				}

				short ProjectileCount = snapshotReadShort(&snap);
				std::cout << "ProjectileCount = " << ProjectileCount << std::endl;
				for (short j = 0; j < ProjectileCount; ++j)
				{
					short ProjectileID = snapshotReadShort(&snap);
					std::cout << "ProjectileID = " << ProjectileID << std::endl;
					short ProjectileType = snapshotReadShort(&snap);
					std::cout << "ProjectileType = " << ProjectileType << std::endl;
					Vector2f ProjectilePos{ snapshotReadFloat(&snap), snapshotReadFloat(&snap) };
					std::cout << "ProjectilePos = " << ProjectilePos.x << "," << ProjectilePos.y << std::endl;
					short DataLength = snapshotReadShort(&snap);
					std::cout << "DataLength = " << DataLength << std::endl;
					if (DataLength > 0)
					{
						if (ProjectileType == 0)
						{
							std::cout << "read ProjectileType = 0" << std::endl;
						}
						else if (ProjectileType == 1)
						{
							std::cout << "read ProjectileType = 1" << std::endl;
						}
						else if (ProjectileType == 2)
						{
							std::cout << "read ProjectileType = 2" << std::endl;
						}
						for (int i = 0; i < DataLength; ++i)
						{
							snapshotReadChar(&snap);
						}
					}
				}
			}
		}
		*/

		processInitPacketQueue();

		if (!pause)
		{
			Vector2f playerVelocity(0, 0);
			float speed = player.getSpeed();
			//Player Movement
			if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
				playerVelocity.x -= speed;
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
				playerVelocity.x += speed;
			if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
				playerVelocity.y -= speed;
			if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
				playerVelocity.y += speed;

			if (magnitude(playerVelocity) > 0)
			{			
				//send to server player input then predict movement on client side
				input.deltaX = playerVelocity.x;
				input.deltaY = playerVelocity.y;
				sendPlayerInputToServer(input);

				player.move(playerVelocity, customWalls);
			}
		}

		for (auto &p : players)
		{
			p.updatePosition();
			p.updateAnimation();
		}
		player.updatePosition();
		player.updateAnimation();

		window.clear(Color::White);

		for (const auto &w : customWalls)
			window.draw(w);

		if (valid)
			window.draw(r);

		window.draw(hintText);

		for (const auto &p : players)
			p.draw(window);
		
		player.draw(window);

		window.display();
	}
	//free(receiveBuffer);
}

void waitRoom(RenderWindow &window)
{
	sf::Text waitText;
	waitText.setString("Waiting for map creation by host...");
	waitText.setColor(Color::Black);
	waitText.setFont(font);
	waitText.setOrigin(Vector2f(waitText.getGlobalBounds().width / 2.f, waitText.getGlobalBounds().height / 2.f));
	waitText.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));

	while (!roomReady && window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (waitText.getString() == "Waiting for map creation by host...")
		{
			waitText.setString("Waiting for map creation by host");
		}
		else
		{
			waitText.setString(waitText.getString() + ".");
		}

		processInitPacketQueue();

		window.clear(Color::White);
		window.draw(waitText);
		window.display();
	}
}

void PlayerGameplay(RenderWindow &window)
{
	sf::Music playerGameplayBgm;
	playerGameplayBgm.openFromFile("Resources\\Music\\Doll_Judgment.ogg");
	playerGameplayBgm.setVolume(8);
	playerGameplayBgm.setLoop(true);
	playerGameplayBgm.play();

	UdpSendSocket.setBlocking(false);
	UdpSendSocket.bind(sf::Socket::AnyPort);
	UdpReceiveSocket.setBlocking(false);
	if (UdpReceiveSocket.bind(5002) != sf::Socket::Done)
		UdpReceiveSocket.bind(5003);//for testing

	Texture ayaTexture;
	Texture cirnoTexture;
	Texture flandreTexture;
	Texture sakuyaTexture;
	Texture suikaTexture;
	Texture daiyouseiTexture;
	ayaTexture.loadFromFile("Resources\\Sprites\\Aya Shameimaru.png");
	cirnoTexture.loadFromFile("Resources\\Sprites\\Cirno.png");
	flandreTexture.loadFromFile("Resources\\Sprites\\Flandre Scarlet.png");
	sakuyaTexture.loadFromFile("Resources\\Sprites\\Sakuya Izayoi.png");
	suikaTexture.loadFromFile("Resources\\Sprites\\Suika Ibuki.png");
	daiyouseiTexture.loadFromFile("Resources\\Sprites\\Daiyousei.png");

	player.setMoveAnimation(flandreTexture, 0.1f);
	player.setAttackAnimation(flandreTexture, 0.15f);
	player.setRangeAnimation(flandreTexture, 0.1f);
	player.setRangeAnimation2(daiyouseiTexture, 0.1f);
	player.setPosition(player.getPosition());

	for (auto &p : players)
	{
		p.setMoveAnimation(flandreTexture, 0.1f);
		p.setAttackAnimation(flandreTexture, 0.15f);
		p.setRangeAnimation(flandreTexture, 0.1f);
		p.setRangeAnimation2(daiyouseiTexture, 0.1f);
		p.setPosition(p.getPosition());
	}
	
	straightProjAni = Animation(flandreTexture, 128, 320, 4, 32, 48, 0.1f);
	SpiralProjAni = Animation(daiyouseiTexture, 128, 280, 6, 32, 40, 0.1f);

	createPlayArea(std::ref(window));

	Vector2f lastMousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

	bool pause = false;

	while (window.isOpen())
	{
		PlayerFrameInput input = { myPlayerNumber, 0.f, 0.f, 0 };
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				TcpSocketToServer.disconnect();
				window.close();
			}
			if (event.type == Event::LostFocus)
				pause = true;
			if (event.type == Event::GainedFocus)
				pause = false;
		}

		if (player.getCurrentHP() <= 0)
		{
			playerGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Lose!");
			return;
		}
		else if ([&]() {
			for (const auto &p : players) {
				if (p.getCurrentHP() > 0)
					return false;
			}
			return true;
		}())
		{
			playerGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Win!");
			return;
		}

		processInitPacketQueue();

		if (!pause)
		{
			Vector2f playerVelocity(0, 0);
			float speed = player.getSpeed();
			//Player Movement
			if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
				playerVelocity.x -= speed;
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
				playerVelocity.x += speed;
			if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
				playerVelocity.y -= speed;
			if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
				playerVelocity.y += speed;
			if (magnitude(playerVelocity) > 0)
			{
				//set player input then predict movement on client side
				input.deltaX = playerVelocity.x;
				input.deltaY = playerVelocity.y;

				player.move(playerVelocity, customWalls);
			}



			//check for attack input
			if (Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Numpad0))
			{
				/*
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				auto result = player.rangeAttack(mouseV);
				if (result)
				{
					std::string s{ "PROJECTILE_NORMAL" };
					s += ";" + std::to_string(myPlayerNumber) + ";" + to_string(mouseV.x) + "," + to_string(mouseV.y);
					sendSocketMessageToServer(s);
				}
				*/

				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				input.targetX = mouseV.x;
				input.targetY = mouseV.y;
				input.SkillUsedMask |= 1;
			}
			if (Keyboard::isKeyPressed(Keyboard::Z))
			{
				/*
				auto result = player.shootSpiral();
				if (result)
				{
					std::string s{ "PROJECTILE_SPIRAL" };
					s += ";" + std::to_string(myPlayerNumber);
					sendSocketMessageToServer(s);
				}
				*/
				input.SkillUsedMask |= 2;
			}
			if (Keyboard::isKeyPressed(Keyboard::X))
			{
				/*
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				auto result = player.shootExpand(mouseV);
				if (result)
				{
					std::string s{ "PROJECTILE_EXPAND" };
					s += ";" + std::to_string(myPlayerNumber) + ";" + to_string(mouseV.x) + "," + to_string(mouseV.y);
					sendSocketMessageToServer(s);
				}
				*/
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				input.targetX = mouseV.x;
				input.targetY = mouseV.y;
				input.SkillUsedMask |= 4;
			}

			sendPlayerInputToServer(input);
		}
		player.updateAnimation();
		player.updatePosition();

		//TODO: process udp messages
		//processPlayerPacketQueue(player2);

		for (auto &p : players)
		{
			p.updateAnimation();
			p.updatePosition();
		}

		//current mouse coordinates
		Vector2f mouseCoord = window.mapPixelToCoords(Mouse::getPosition(window));

		//current player position
		Vector2f playerPosition = player.getPosition() + Vector2f(16, 20);
		Vector2f PQ;

		//get vector from player position to mouse position
		if (playerPosition == mouseCoord)
			mouseCoord = lastMousePosition;
		PQ = lastMousePosition - playerPosition;

		//mouse position too close to player position. project mouse position onto surface of circle
		if (magnitude(PQ) < 50)
			PQ = (PQ / magnitude(PQ)) * 50.f;
		lastMousePosition = mouseCoord;

		//get the left and right vectors of the player view area (left and right LOS arms) by rotating vector PQ by 15 degrees
		Vector2f rightArm = rotateVector2f(PQ, 35) * 100.0f;
		Vector2f leftArm = rotateVector2f(PQ, -35) * 100.0f;

		//get los ConvexShapes
		std::vector<ConvexShape> los = clipLOS(playerPosition, lastMousePosition, leftArm, rightArm);

		//check shadow ConvexShapes
		std::vector<ConvexShape> shadow = checkShadow(customWalls, playerPosition);

		window.clear(Color::White);

		//player.updateProjectile();

		for (auto &p : players)
		{
			//p.updateProjectile();
			p.draw(window);
			//p.drawProjectile(window);
		}

		for (auto it = projMap.begin(); it != projMap.end(); )
		{
			if (!it->second->updateProjectile())
			{
				it = projMap.erase(it);
			}
			else
			{
				it->second->updateAnimation();
				it->second->updateSpritePosition();
				it->second->draw(window);
				++it;
			}
		}

		//player.drawProjectile(window);

		for (const auto &s : shadow)
			window.draw(s);

		for (const auto &i : los)
			window.draw(i);

		for (const auto &w : customWalls)
			window.draw(w);

		player.draw(window);

		window.display();
	}
	playerGameplayBgm.stop();
	return;
}

void GameLobby(RenderWindow &window, std::string ip)
{
	sf::Text IPHolder;
	IPHolder.setString("Your IP Address is " + ip);
	IPHolder.setColor(Color::Black);
	IPHolder.setFont(font);
	IPHolder.setOrigin(Vector2f(IPHolder.getGlobalBounds().width / 2.f, IPHolder.getGlobalBounds().height / 2.f));
	IPHolder.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 10.f));

	sf::Text waitingText;
	waitingText.setString("Waiting for players...");
	waitingText.setColor(Color::Black);
	waitingText.setFont(font);
	waitingText.setOrigin(Vector2f(waitingText.getGlobalBounds().width / 2.f, waitingText.getGlobalBounds().height / 2.f));
	waitingText.setPosition(IPHolder.getPosition() + Vector2f(0, 50));

	sf::Text readyText;
	readyText.setString("Ready");
	readyText.setColor(Color::Black);
	readyText.setFont(font);
	readyText.setOrigin(Vector2f(readyText.getGlobalBounds().width / 2.f, readyText.getGlobalBounds().height / 2.f));
	readyText.setPosition(static_cast<Vector2f>(window.getSize()) - 4.f*readyText.getOrigin());

	sf::Text backText;
	backText.setString("Back");
	backText.setColor(Color::Black);
	backText.setFont(font);
	backText.setOrigin(Vector2f(backText.getGlobalBounds().width / 2.f, backText.getGlobalBounds().height / 2.f));
	backText.setPosition(readyText.getPosition() + Vector2f(100, 0));

	while (!gameReady)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
					if (readyText.getGlobalBounds().contains(mousePos))
					{
						std::string s{ "PLAYER_READY" };
						s += ";" + std::to_string(myPlayerNumber);
						sendSocketMessageToServer(s);
					}
					else if (backText.getGlobalBounds().contains(mousePos))
					{
						std::string s{ "PLAYER_DISCONNECTED" };
						s += ";" + std::to_string(myPlayerNumber);
						sendSocketMessageToServer(s);
						tryListen = false;
						fadeScreen(std::ref(window), 1.5);
						return;
					}
				}
			}
		}
		
		processInitPacketQueue();

		if (waitingText.getString() == "Waiting for players...")
		{
			waitingText.setString("Waiting for players");
		}
		else
		{
			waitingText.setString(waitingText.getString() + ".");
		}

		window.clear(Color::White);
		window.draw(IPHolder);
		window.draw(waitingText);
		window.draw(readyText);
		window.draw(backText);
		window.display();
	}
	tryListen = false;
	fadeScreen(std::ref(window), 1.5);
	BGM.stop();
	PlayerGameplay(std::ref(window));
	BGM.play();

}


void CreateRoom(RenderWindow &window)
{
	Server server;
	std::thread serverThread(&Server::startServer, &server);

	auto ip = sf::IpAddress::getPublicAddress().toString();
	ConnectingRoom(std::ref(window), ip);

	server.stop();
	serverThread.join();
}

//create fade screen effect with duration in seconds
void fadeScreen(RenderWindow &window, float seconds)
{
	RectangleShape fade(static_cast<Vector2f>(window.getSize()));
	float a = 0.f;
	sf::Image i = window.capture();
	sf::Texture t;
	t.loadFromImage(i);
	Sprite s;
	s.setTexture(t);

	float step = 255.f / (60.f * seconds);
	while (a < 255)
	{
		fade.setFillColor(sf::Color(0, 0, 0, static_cast<int>(a)));
		a = std::fmin(255.f, a + step);
		window.draw(s);
		window.draw(fade);
		window.display();
	}
}

bool ConnectingRoom(RenderWindow &window, std::string ipAddr)
{
	tryConnect = true;
	std::thread clientThread(&client, ipAddr);

	sf::Text IPHolder;
	IPHolder.setString("Connecting to IP " + ipAddr);
	IPHolder.setColor(Color::Black);
	IPHolder.setFont(font);
	IPHolder.setOrigin(Vector2f(IPHolder.getGlobalBounds().width / 2.f, IPHolder.getGlobalBounds().height / 2.f));
	IPHolder.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));

	sf::Text connectingText;
	connectingText.setString("Joining Room...");
	connectingText.setColor(Color::Black);
	connectingText.setFont(font);
	connectingText.setOrigin(Vector2f(connectingText.getGlobalBounds().width / 2.f, connectingText.getGlobalBounds().height / 2.f));
	connectingText.setPosition(IPHolder.getPosition() + Vector2f(0, 50));

	sf::Text backText;
	backText.setString("Back");
	backText.setColor(Color::Black);
	backText.setFont(font);
	backText.setOrigin(Vector2f(backText.getGlobalBounds().width / 2.f, backText.getGlobalBounds().height / 2.f));
	backText.setPosition(connectingText.getPosition() + Vector2f(0, 50));

	while (!connected)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
					if (backText.getGlobalBounds().contains(mousePos))
					{
						tryConnect = false;
						fadeScreen(std::ref(window), 1.5);
						clientThread.join();
						return false;
					}
				}
			}
		}

		if (connectingText.getString() == "Joining Room...")
		{
			connectingText.setString("Joining Room");
		}
		else
		{
			connectingText.setString(connectingText.getString() + ".");
		}

		window.clear(Color::White);
		window.draw(IPHolder);
		window.draw(connectingText);
		window.draw(backText);
		window.display();
	}

	//std::thread receiveThread(&receiveMessageFromServer);
	GameLobby(std::ref(window), hostAddress);
	//receiveThread.join();
	return true;
}

void JoinRoom(RenderWindow &window)
{
	sf::Text TextPrompt;
	TextPrompt.setString("Please enter IP to connect...");
	TextPrompt.setColor(Color::Black);
	TextPrompt.setFont(font);
	TextPrompt.setOrigin(Vector2f(TextPrompt.getGlobalBounds().width / 2.f, TextPrompt.getGlobalBounds().height / 2.f));
	TextPrompt.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));

	sf::Text IP;
	IP.setString("");
	IP.setColor(Color::Black);
	IP.setFont(font);
	IP.setOrigin(Vector2f(IP.getGlobalBounds().width / 2.f, IP.getGlobalBounds().height / 2.f));
	IP.setPosition(TextPrompt.getPosition() + Vector2f(0, 50));

	sf::Text connectText;
	connectText.setString("Connect!");
	connectText.setColor(Color::Black);
	connectText.setFont(font);
	connectText.setOrigin(Vector2f(connectText.getGlobalBounds().width / 2.f, connectText.getGlobalBounds().height / 2.f));
	connectText.setPosition(IP.getPosition() + Vector2f(0, 50));

	sf::Text backText;
	backText.setString("Back");
	backText.setColor(Color::Black);
	backText.setFont(font);
	backText.setOrigin(Vector2f(backText.getGlobalBounds().width / 2.f, backText.getGlobalBounds().height / 2.f));
	backText.setPosition(connectText.getPosition() + Vector2f(0, 50));

	std::string ipstring;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
					if (backText.getGlobalBounds().contains(mousePos))
					{
						return;
					}
					else if (connectText.getGlobalBounds().contains(mousePos))
					{
						//window.setActive(false);
						auto result = ConnectingRoom(std::ref(window), ipstring);
						if (result)
							return;
					}
				}
			}
			else if (event.type == sf::Event::TextEntered)
			{
				//if backspace pressed...
				if (event.text.unicode == 8)
				{
					if (!ipstring.empty())
						ipstring.pop_back();
				}
				//if enter pressed...
				else if (event.text.unicode == 13)
				{
					//window.setActive(false);
					auto result = ConnectingRoom(std::ref(window), ipstring);
					if (result)
						return;
				}
				else if (event.text.unicode < 128)
				{
					ipstring.push_back(static_cast<char>(event.text.unicode));
				}
			}
		}

		IP.setString(ipstring);
		IP.setOrigin(Vector2f(IP.getGlobalBounds().width / 2.f, IP.getGlobalBounds().height / 2.f));
		IP.setPosition(TextPrompt.getPosition() + Vector2f(0, 50));

		window.clear(Color::White);
		window.draw(TextPrompt);
		window.draw(IP);
		window.draw(backText);
		window.draw(connectText);
		window.display();
	}
	//serverThread->wait();

	//window.setActive(false);
	//PlayerGameplay(std::ref(window));
}

void AIGameplay(RenderWindow &window)
{
	sf::Music AIGameplayBgm;
	AIGameplayBgm.openFromFile("Resources\\Music\\Doll_Judgment.ogg");
	AIGameplayBgm.setVolume(8);
	AIGameplayBgm.setLoop(true);
	AIGameplayBgm.play();

	std::vector<double> mapMatrix(static_cast<size_t>(windowWidth / 10) * static_cast<size_t>(windowHeight / 10), 1);
	std::vector<int> workVector((windowWidth / 10) * (windowHeight / 10), 0);

	Clock clock;
	float pathTime = clock.getElapsedTime().asSeconds();

	std::vector<RectangleShape> walls;

	//create the walls
	RectangleShape rect1;
	rect1.setSize(Vector2f(80, 220));
	rect1.setPosition(Vector2f(120, 110));
	rect1.setTexture(&brickTexture);
	rect1.setTextureRect({0,0,800,2200});
	walls.push_back(rect1);
	updateMapMatrix(mapMatrix, rect1.getPosition(), rect1.getPosition() + rect1.getSize(), -1);

	RectangleShape rect2;
	rect2.setSize(Vector2f(80, 300));
	rect2.setPosition(Vector2f(520, 250));
	rect2.setTexture(&brickTexture);
	rect2.setTextureRect({ 0,0,800,3000 });
	walls.push_back(rect2);
	updateMapMatrix(mapMatrix, rect2.getPosition(), rect2.getPosition() + rect2.getSize(), -1);

	RectangleShape rect3;
	rect3.setSize(Vector2f(60, 160));
	rect3.setPosition(Vector2f(300, 50));
	rect3.setTexture(&brickTexture);
	rect3.setTextureRect({ 0,0,600,1600 });
	walls.push_back(rect3);
	updateMapMatrix(mapMatrix, rect3.getPosition(), rect3.getPosition() + rect3.getSize(), -1);

	RectangleShape rect4;
	rect4.setSize(Vector2f(250, 110));
	rect4.setPosition(Vector2f(750, 600));
	rect4.setTexture(&brickTexture);
	rect4.setTextureRect({ 0,0,2500,1100 });
	walls.push_back(rect4);
	updateMapMatrix(mapMatrix, rect4.getPosition(), rect4.getPosition() + rect4.getSize(), -1);

	RectangleShape rect5;
	rect5.setSize(Vector2f(220, 90));
	rect5.setPosition(Vector2f(1000, 320));
	rect5.setTexture(&brickTexture);
	rect5.setTextureRect({ 0,0,2200,900 });
	walls.push_back(rect5);
	updateMapMatrix(mapMatrix, rect5.getPosition(), rect5.getPosition() + rect5.getSize(), -1);

	Player singlePlayer(Vector2f(32, 40), Vector2f(windowWidth / 2.f, windowHeight / 2.f));

	Vector2f lastMousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

	std::vector<std::unique_ptr<Enemy>> enemyVector;

	Texture ayaTexture;
	Texture cirnoTexture;
	Texture flandreTexture;
	Texture sakuyaTexture;
	Texture suikaTexture;
	Texture daiyouseiTexture;
	ayaTexture.loadFromFile("Resources\\Sprites\\Aya Shameimaru.png");
	cirnoTexture.loadFromFile("Resources\\Sprites\\Cirno.png");
	flandreTexture.loadFromFile("Resources\\Sprites\\Flandre Scarlet.png");
	sakuyaTexture.loadFromFile("Resources\\Sprites\\Sakuya Izayoi.png");
	suikaTexture.loadFromFile("Resources\\Sprites\\Suika Ibuki.png");
	daiyouseiTexture.loadFromFile("Resources\\Sprites\\Daiyousei.png");

	std::unique_ptr<Enemy> aya = std::make_unique<Aya>(Vector2f(1000, 20));
	aya->setMoveAnimation(ayaTexture, 0.1f);
	aya->setAttackAnimation(ayaTexture, 0.15f);
	enemyVector.push_back(std::move(aya));

	std::unique_ptr<Enemy> cirno = std::make_unique<Cirno>(Vector2f(20, 20));
	cirno->setMoveAnimation(cirnoTexture, 0.1f);
	cirno->setAttackAnimation(cirnoTexture, 0.15f);
	enemyVector.push_back(std::move(cirno));

	std::unique_ptr<Enemy> sakuya = std::make_unique<Sakuya>(Vector2f(1000, 500));
	sakuya->setMoveAnimation(sakuyaTexture, 0.1f);
	sakuya->setAttackAnimation(sakuyaTexture, 0.1f);
	sakuya->setRangeAnimation(sakuyaTexture, 0.1f);
	enemyVector.push_back(std::move(sakuya));

	std::unique_ptr<Enemy> suika = std::make_unique<Suika>(Vector2f(500, 450));
	suika->setMoveAnimation(suikaTexture, 0.1f);
	suika->setAttackAnimation(suikaTexture, 0.15f);
	enemyVector.push_back(std::move(suika));

	singlePlayer.setMoveAnimation(flandreTexture, 0.1f);
	singlePlayer.setAttackAnimation(flandreTexture, 0.15f);
	singlePlayer.setRangeAnimation(flandreTexture, 0.1f);
	singlePlayer.setRangeAnimation2(daiyouseiTexture, 0.1f);
	singlePlayer.setPosition(singlePlayer.getPosition());

	std::vector<int> dirtyWalls(walls.size(), 0);
	size_t currentWall = walls.size();
	float deltaX;
	float deltaY;

	bool pause = false;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::LostFocus)
				pause = true;
			if (event.type == Event::GainedFocus)
				pause = false;
		}

		if (singlePlayer.getCurrentHP() <= 0)
		{
			AIGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Lose!");
			return;
		}

		if (enemyVector.empty())
		{
			AIGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Win!");
			return;
		}

		if (!pause)
		{
			//allow walls to be moved around by dragging with MLB
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				if (currentWall == walls.size())
				{
					for (size_t i = 0; i < walls.size(); ++i)
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
					updateMapMatrix(mapMatrix, walls[currentWall].getPosition(), walls[currentWall].getPosition() + wallSize, 1);

				//move wall to new mouse position
				float posX = window.mapPixelToCoords(Mouse::getPosition(window)).x - deltaX;
				float posY = window.mapPixelToCoords(Mouse::getPosition(window)).y - deltaY;

				if (posX < 0)
					posX = 0;
				else if (posX + wallSize.x > windowWidth)
					posX = windowWidth - wallSize.x;
				if (posY < 0)
					posY = 0;
				else if (posY + wallSize.y > windowHeight)
					posY = windowHeight - wallSize.y;

				walls[currentWall].setPosition(Vector2f(posX, posY));

				//set Wall to dirty and to be updated in pathfinding
				dirtyWalls[currentWall] = 1;
			}

			Vector2f playerVelocity(0, 0);
			float speed = singlePlayer.getSpeed();
			//Player Movement
			if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
				playerVelocity.x -= speed;
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
				playerVelocity.x += speed;
			if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
				playerVelocity.y -= speed;
			if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
				playerVelocity.y += speed;
			if (magnitude(playerVelocity) > 0)
				singlePlayer.move(playerVelocity, walls);

			//check for attack input
			if (Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Numpad0))
			{
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				singlePlayer.rangeAttack(mouseV);
			}
			if (Keyboard::isKeyPressed(Keyboard::Z))
			{
				singlePlayer.shootSpiral();
			}
			if (Keyboard::isKeyPressed(Keyboard::X))
			{
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				singlePlayer.shootExpand(mouseV);
			}
		}

		singlePlayer.updateAnimation();
		singlePlayer.updatePosition();

		//current mouse coordinates
		Vector2f mouseCoord = window.mapPixelToCoords(Mouse::getPosition(window));

		//current player position
		Vector2f playerPosition = singlePlayer.getPosition() + Vector2f(16, 20);
		Vector2f PQ;

		//get vector from player position to mouse position
		if (playerPosition == mouseCoord)
			mouseCoord = lastMousePosition;
		PQ = lastMousePosition - playerPosition;

		//mouse position too close to player position. project mouse position onto surface of circle
		if (magnitude(PQ) < 50)
			PQ = (PQ / magnitude(PQ)) * 50.f;
		lastMousePosition = mouseCoord;

		//get the left and right vectors of the player view area (left and right LOS arms) by rotating vector PQ by 15 degrees
		Vector2f rightArm = rotateVector2f(PQ, 35) * 100.0f;
		Vector2f leftArm = rotateVector2f(PQ, -35) * 100.0f;

		//get los ConvexShapes
		std::vector<ConvexShape> los = clipLOS(playerPosition, lastMousePosition, leftArm, rightArm);

		//check shadow ConvexShapes
		std::vector<ConvexShape> shadow = checkShadow(walls, playerPosition);

		//pathfinding
		if (clock.getElapsedTime().asSeconds() - pathTime > 1)
		{
			//check if walls are moved and update mapMatrix accordingly
			for (size_t i = 0; i < dirtyWalls.size(); ++i)
			{
				if (dirtyWalls[i] == 1)
				{
					updateMapMatrix(mapMatrix, walls[i].getPosition(), walls[i].getPosition() + walls[i].getSize(), -1);
					dirtyWalls[i] = 0;
				}
			}
			for (const auto &e : enemyVector)
			{
				if ((e->ranged() && e->inRange(singlePlayer.getPosition())) || e->getBounds().intersects(singlePlayer.getBounds()))
					continue;
				e->enemyPathfinder(mapMatrix, singlePlayer.getPosition(), workVector);
			}
			pathTime = clock.getElapsedTime().asSeconds();
		}

		//draw background color
		window.clear(Color::White);

		//do everything for enemies
		for (int i = 0; i < enemyVector.size(); ++i)
		{
			if (enemyVector[i]->getCurrentHP() <= 0)
			{
				enemyVector.erase(enemyVector.begin() + i);
				--i;
			}
			else
			{
				enemyVector[i]->updateEnemy(enemyVector);
				if (enemyVector[i]->getBounds().intersects(singlePlayer.getBounds()))
				{
					//reached
					enemyVector[i]->targetReached();
					//do melee atk
					enemyVector[i]->meleeAttack(playerPosition);
				}
				else if (enemyVector[i]->ranged() && enemyVector[i]->inRange(playerPosition))
				{
					//reached range
					enemyVector[i]->targetReached();
					//do range attack
					enemyVector[i]->rangeAttack(playerPosition);
				}

				enemyVector[i]->updateAnimation(singlePlayer);
				enemyVector[i]->updateSpritePosition();
				enemyVector[i]->drawEnemy(window);
			}
		}

		//update enemy projectiles and draw them
		for (const auto &e : enemyVector)
		{
			e->calcProjCollision(singlePlayer);
			e->updateProjectile();
			e->drawProjectiles(window);
		}

		//handle player projectile collision
		singlePlayer.calcProjCollision(enemyVector);

		//update player projectiles
		singlePlayer.updateProjectile();

		//draw shadow first
		for (const auto &s : shadow)
			window.draw(s);

		//draw player view area by drawing the opposite
		for (const auto &l : los)
			window.draw(l);

		//draw walls
		for (const auto &w : walls)
			window.draw(w);

		//draw player
		singlePlayer.draw(window);

		//draw player projectiles
		singlePlayer.drawProjectile(window);

		window.display();
	}
	AIGameplayBgm.stop();
}

void startScreen(RenderWindow &window)
{
	BGM.openFromFile("Resources\\Music\\Necrofantasia.ogg");
	BGM.setVolume(6);
	BGM.setLoop(true);
	BGM.play();

	brickTexture.loadFromFile("Resources\\Textures\\mosswallTexture.jpg");
	brickTexture.setRepeated(true);
	font.loadFromFile("Resources\\Fonts\\LLPIXEL3.ttf");

	sf::Text menu1Text;
	menu1Text.setString("VS AI");
	menu1Text.setColor(Color::Black);
	menu1Text.setFont(font);
	menu1Text.setOrigin(Vector2f(menu1Text.getGlobalBounds().width / 2.f, menu1Text.getGlobalBounds().height / 2.f));
	menu1Text.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));

	sf::Text menu2Text;
	menu2Text.setString("VS Player - Create Room");
	menu2Text.setColor(Color::Black);
	menu2Text.setFont(font);
	menu2Text.setOrigin(Vector2f(menu2Text.getGlobalBounds().width / 2.f, menu2Text.getGlobalBounds().height / 2.f));
	menu2Text.setPosition(menu1Text.getPosition() + Vector2f(0,50));

	sf::Text menu3Text;
	menu3Text.setString("VS Player - Join Room");
	menu3Text.setColor(Color::Black);
	menu3Text.setFont(font);
	menu3Text.setOrigin(Vector2f(menu3Text.getGlobalBounds().width / 2.f, menu3Text.getGlobalBounds().height / 2.f));
	menu3Text.setPosition(menu2Text.getPosition() + Vector2f(0, 50));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					auto mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
					if (menu1Text.getGlobalBounds().contains(mousePos))
					{
						BGM.stop();
						AIGameplay(std::ref(window));
						BGM.play();
					}
					else if (menu2Text.getGlobalBounds().contains(mousePos))
					{
						CreateRoom(std::ref(window));
					}
					else if (menu3Text.getGlobalBounds().contains(mousePos))
					{
						JoinRoom(std::ref(window));
					}
				}
			}
		}

		window.clear(Color::White);
		window.draw(menu1Text);
		window.draw(menu2Text);
		window.draw(menu3Text);
		window.display();
	}
}

int main()
{
	windowWidth = 1280.f;
	windowHeight = 720.f;

	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(windowWidth, windowHeight), "FogShooter", Style::Default, settings);
	window.setFramerateLimit(60);
	//window.setVerticalSyncEnabled(true);

	startScreen(std::ref(window));

	return 0;
}


/*
*FIXED* - ISSUE#1: pressing back from "connecting room" with valid IP results in freeze due to client thread not terminated
*FIXED* (assuming issue was serverThread not ending) - ISSUE#2: game doesnt close properly sometimes
*FIXED* - ISSUE#3: disconnect server after pressing back on createRoom()
*FIXED* - ISSUE#4: crash on winning
ISSUE#5: packetQueue need mutex
*DONE* - ISSUE#6: add retry after game over

TODO: Process server message queue
*/