#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <winsock2.h>
#include <vector>
#include <stack>
#include <queue>
#include <thread>
#include "main.h"

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
sf::TcpSocket tcpsocket;
bool connected = false;
bool tryConnect = true;
bool tryListen = true;
bool roomReady = false;
bool opponentLeft = false;

int counter = 0;
int counter2 = 0;
std::queue<std::string> packetQueue;
std::vector<RectangleShape> customWalls;
sf::Font font;
sf::Music BGM;
sf::Texture brickTexture;

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

void server()
{
	std::cout << "server thread start here" << std::endl;
	//std::cout << "server running... IP is " << sf::IpAddress::getLocalAddress().toString() << std::endl;
	std::cout << "server running... IP is " << sf::IpAddress::getPublicAddress().toString() << std::endl;
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
				if (listener.accept(tcpsocket) == sf::Socket::Done)
				{
					std::cout << "socket accepted on ip " << tcpsocket.getRemoteAddress().toString() << std::endl;
					connected = true;
				}
			}
		}
	}
	std::cout << "server thread ends here" << std::endl;
}

void client(std::string ipAddr)
{
	std::cout << "client thread start here" << std::endl;
	if (ipAddr == sf::IpAddress::getPublicAddress().toString())
	{
		std::cout << "Attempting connection to own IP not allowed. Client thread end here" << std::endl;
		return;
	}

	sf::IpAddress ip;
	if (ipAddr.empty())
		ip = sf::IpAddress();
	else
		ip = sf::IpAddress(ipAddr);
	std::cout << "Attemping conenction on ip " << ip.toString() << std::endl;

	//try connect with incremental timeout upto 2 seconds
	auto timeout = sf::milliseconds(200);
	auto status = tcpsocket.connect(ip, PORT, timeout);
	while (tryConnect && status != sf::Socket::Done)
	{
		std::cout << "Could not connect. Status code: " << status << ". Trying again with timeout = " << timeout.asMilliseconds() << "..." << std::endl;
		timeout = sf::milliseconds(std::min(2000, 2 * timeout.asMilliseconds()));
		tcpsocket.connect(ip, PORT, timeout);
	}
	if (status == sf::Socket::Done)
	{
		std::cout << "Connected on ip " << tcpsocket.getRemoteAddress() << std::endl;
		connected = true;
	}
	std::cout << "Client thread ends here" << std::endl;
}

void receivemsg()
{
	std::cout << "receivemsg thread start here" << std::endl;
	while (true)
	{
		sf::Packet packet;
		auto status = tcpsocket.receive(packet);
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
	std::cout << "receivemsg thread ends here" << std::endl;
}

void sendSocketMessage(std::string message)
{
	sf::Packet packet;
	packet << message;
	
	while (tcpsocket.send(packet) == sf::Socket::Partial);
	++counter2;
	std::cout << "sent " << counter2 << " packets" << std::endl;
}

void processInitPacketQueue()
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
		if (code == "ROOM_READY")
		{
			std::cout << "received packet ROOM_READY!" << std::endl;
			i++;
			while (i != s.end())
			{
				string posX;
				string posY;
				string sizeX;
				string sizeY;
				for (; i != s.end() && *i != ','; ++i)
				{
					posX += *i;
				}
				++i;
				for (; i != s.end() && *i != ';'; ++i)
				{
					posY += *i;
				}
				++i;
				for (; i != s.end() && *i != ','; ++i)
				{
					sizeX += *i;
				}
				++i;
				for (; i != s.end() && *i != ';'; ++i)
				{
					sizeY += *i;
				}
				RectangleShape r{ Vector2f{ static_cast<float>(atof(sizeX.c_str())), static_cast<float>(atof(sizeY.c_str())) } };
				r.setPosition(Vector2f{ static_cast<float>(atof(posX.c_str())), static_cast<float>(atof(posY.c_str())) });
				//r.setFillColor(Color::Black);
				r.setTexture(&brickTexture);
				r.setTextureRect({ 0,0,10 * static_cast<int>(r.getSize().x),10 * static_cast<int>(r.getSize().y) });
				customWalls.emplace_back(r);
				std::cout << "Rectangle at (" << r.getPosition().x << "," << r.getPosition().y << ") with size (" << r.getSize().x << "," << r.getSize().y << ")" << std::endl;
				if (i == s.end())
					break;
				else 
					i++;
			}
			roomReady = true;
		}
	}
}

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

void GameOverScreen(RenderWindow &window, std::string str)
{
	tcpsocket.disconnect();

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
	gameOverText.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));

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
}

void createPlayArea(RenderWindow &window, Player &player1, Player &player2)
{
	sf::Text hintText;
	hintText.setString("Click and drag to create walls, and press Enter to confirm!");
	hintText.setColor(Color::Black);
	hintText.setFont(font);
	hintText.setOrigin(Vector2f(hintText.getGlobalBounds().width / 2.f, hintText.getGlobalBounds().height / 2.f));
	hintText.setPosition(Vector2f(windowWidth / 2.f,  50));

	Vector2f upperLeft;
	Vector2f bottomRight;
	
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					upperLeft.x = event.mouseButton.x; 
					upperLeft.y = event.mouseButton.y;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					Vector2f v = { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };
					for (int i = 0; i < customWalls.size(); ++i)
					{
						if (customWalls[i].getGlobalBounds().contains(v))
						{
							customWalls.erase(customWalls.begin() + i);
							break;
						}
					}
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					bottomRight.x = event.mouseButton.x;
					bottomRight.y = event.mouseButton.y;
					auto size = bottomRight - upperLeft;
					if (size.x >= 5 && size.y >= 5)
					{
						RectangleShape s{ size };
						s.setPosition(upperLeft);
						//s.setFillColor(Color::Black);
						if (!player1.getBounds().intersects(s.getGlobalBounds()) && !player2.getBounds().intersects(s.getGlobalBounds()))
						{
							s.setTexture(&brickTexture);
							s.setTextureRect({ 0,0,10 * static_cast<int>(s.getSize().x),10 * static_cast<int>(s.getSize().y) });
							customWalls.emplace_back(s);
						}
					}
				}
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape ||
					event.key.code == sf::Keyboard::Return)
				{
					std::string s{"ROOM_READY"};
					for (const auto &i : customWalls)
					{
						s += ";" + to_string(i.getPosition().x) + "," + to_string(i.getPosition().y);
						s += ";" + to_string(i.getSize().x) + "," + to_string(i.getSize().y);
						std::cout << "Rectangle at (" << i.getPosition().x << "," << i.getPosition().y << ") with size (" << i.getSize().x << "," << i.getSize().y << ")" << std::endl;
					}
					std::cout << "sent message: " << s << std::endl;
					sendSocketMessage(s);
					return;
				}
			}
		}

		player1.updateAnimation();
		player2.updateAnimation();

		window.clear(Color::White);

		for (const auto &w : customWalls)
			window.draw(w);

		window.draw(hintText);

		player1.draw(window);
		player2.draw(window);

		window.display();
	}
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

void PlayerGameplay(RenderWindow &window, int playerNumber)
{
	sf::Music playerGameplayBgm;
	playerGameplayBgm.openFromFile("Resources\\Music\\Doll_Judgment.ogg");
	playerGameplayBgm.setVolume(10);
	playerGameplayBgm.setLoop(true);
	playerGameplayBgm.play();

	std::thread receiveThread(&receivemsg);
	receiveThread.detach();

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

	Player player1(Vector2f(32, 40), Vector2f((playerNumber == 0) ? 200 : 1000, windowHeight / 2.f));
	player1.setMoveAnimation(flandreTexture, 0.1f);
	player1.setAttackAnimation(flandreTexture, 0.15f);
	player1.setRangeAnimation(flandreTexture, 0.1f);
	player1.setRangeAnimation2(daiyouseiTexture, 0.1f);
	player1.getSprite()->setPosition(player1.getPosition());

	Player player2(Vector2f(32, 40), Vector2f((playerNumber != 0) ? 200 : 1000, windowHeight / 2.f));
	player2.setMoveAnimation(flandreTexture, 0.1f);
	player2.setAttackAnimation(flandreTexture, 0.15f);
	player2.setRangeAnimation(flandreTexture, 0.1f);
	player2.setRangeAnimation2(daiyouseiTexture, 0.1f);
	player2.getSprite()->setPosition(player2.getPosition());
	
	if (playerNumber == 0)
	{
		createPlayArea(std::ref(window), player1, player2);
	}
	else
	{
		waitRoom(std::ref(window));
	}

	Vector2f lastMousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

	bool pause = false;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				tcpsocket.disconnect();
				window.close();
			}
			if (event.type == Event::LostFocus)
				pause = true;
			if (event.type == Event::GainedFocus)
				pause = false;
		}
		
		if (player1.getCurrentHP() <= 0)
		{
			playerGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Lose!");
			return;
		}
		else if (player2.getCurrentHP() <= 0 || opponentLeft)
		{
			playerGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Win!");
			return;
		}

		if (!pause)
		{
			Vector2f playerVelocity(0, 0);
			//Player Movement
			if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
				playerVelocity.x -= 3.f;
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
				playerVelocity.x += 3.f;
			if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
				playerVelocity.y -= 3.f;
			if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
				playerVelocity.y += 3.f;
			if (magnitude(playerVelocity) > 0)
			{
				player1.move(playerVelocity, customWalls);
				std::string s{ "PLAYER_MOVE" };
				s += ";" + to_string(playerVelocity.x) + "," + to_string(playerVelocity.y);
				sendSocketMessage(s);
			}

			//check for attack input
			if (Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Numpad0))
			{
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				auto result = player1.rangeAttack(mouseV);
				if (result)
				{
					std::string s{ "PROJECTILE_NORMAL" };
					s += ";" + to_string(mouseV.x) + "," + to_string(mouseV.y);
					sendSocketMessage(s);
				}
			}
			if (Keyboard::isKeyPressed(Keyboard::Z))
			{
				auto result = player1.shootSpiral();
				if (result)
				{
					std::string s{ "PROJECTILE_SPIRAL" };
					sendSocketMessage(s);
				}
			}
			if (Keyboard::isKeyPressed(Keyboard::X))
			{
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				auto result = player1.shootExpand(mouseV);
				if (result)
				{
					std::string s{ "PROJECTILE_EXPAND" };
					s += ";" + to_string(mouseV.x) + "," + to_string(mouseV.y);
					sendSocketMessage(s);
				}
			}
		}
		player1.updateAnimation();
		player1.updatePosition();

		processPlayerPacketQueue(player2);

		player2.updateAnimation();
		player2.updatePosition();

		float dmg;
		dmg = player1.calcProjCollision(player2.getBounds());
		player2.takeDamage(dmg);

		dmg = player2.calcProjCollision(player1.getBounds());
		player1.takeDamage(dmg);


		//current mouse coordinates
		Vector2f mouseCoord = window.mapPixelToCoords(Mouse::getPosition(window));

		//current player position
		Vector2f playerPosition = player1.getPosition() + Vector2f(16, 20);
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

		player1.updateProjectile();
		player2.updateProjectile();

		player2.draw(window);
		player2.drawProjectile(window);

		player1.drawProjectile(window);

		for (const auto &s : shadow)
			window.draw(s);

		for (const auto &i : los)
			window.draw(i);

		for (const auto &w : customWalls)
			window.draw(w);

		player1.draw(window);

		window.display();
	}
	//receiveThread->terminate();
	playerGameplayBgm.stop();
}

void CreateRoom(RenderWindow &window)
{
	tryListen = true;
	std::thread serverThread(&server);

	//auto ip = sf::IpAddress::getLocalAddress().toString();
	auto ip = sf::IpAddress::getPublicAddress().toString();
	
	sf::Text IPHolder;
	IPHolder.setString("Your IP Address is " + ip);
	IPHolder.setColor(Color::Black);
	IPHolder.setFont(font);
	IPHolder.setOrigin(Vector2f(IPHolder.getGlobalBounds().width / 2.f, IPHolder.getGlobalBounds().height / 2.f));
	IPHolder.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));

	sf::Text connectingText;
	connectingText.setString("Waiting for connection...");
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
						tryListen = false;
						fadeScreen(std::ref(window), 1.5);
						serverThread.join();
						return;
					}
				}
			}
		}

		if (connectingText.getString() == "Waiting for connection...")
		{
			connectingText.setString("Waiting for connection");
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
	serverThread.join();
	//window.setActive(false);
	BGM.stop();
	PlayerGameplay(std::ref(window), 0);
	BGM.play();
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

	//window.setActive(false);
	BGM.stop();
	PlayerGameplay(std::ref(window), 1);
	BGM.play();
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
	AIGameplayBgm.setVolume(10);
	AIGameplayBgm.setLoop(true);
	AIGameplayBgm.play();

	std::vector<double> mapMatrix((windowWidth / 10) * (windowHeight / 10), 1);
	std::vector<int> workVector((windowWidth / 10) * (windowHeight / 10), 0);

	Clock clock;
	float pathTime = clock.getElapsedTime().asSeconds();

	std::vector<RectangleShape> walls;

	//create the walls
	RectangleShape rect1;
	rect1.setSize(Vector2f(80, 220));
	rect1.setPosition(Vector2f(120, 110));
	//rect1.setFillColor(Color::Black);
	rect1.setTexture(&brickTexture);
	rect1.setTextureRect({0,0,800,2200});
	walls.push_back(rect1);
	updateMapMatrix(mapMatrix, rect1.getPosition(), rect1.getPosition() + rect1.getSize(), -1);

	RectangleShape rect2;
	rect2.setSize(Vector2f(80, 300));
	rect2.setPosition(Vector2f(520, 250));
	//rect2.setFillColor(Color::Black);
	rect2.setTexture(&brickTexture);
	rect2.setTextureRect({ 0,0,800,3000 });
	walls.push_back(rect2);
	updateMapMatrix(mapMatrix, rect2.getPosition(), rect2.getPosition() + rect2.getSize(), -1);

	RectangleShape rect3;
	rect3.setSize(Vector2f(60, 160));
	rect3.setPosition(Vector2f(300, 50));
	//rect3.setFillColor(Color::Black);
	rect3.setTexture(&brickTexture);
	rect3.setTextureRect({ 0,0,600,1600 });
	walls.push_back(rect3);
	updateMapMatrix(mapMatrix, rect3.getPosition(), rect3.getPosition() + rect3.getSize(), -1);

	RectangleShape rect4;
	rect4.setSize(Vector2f(250, 110));
	rect4.setPosition(Vector2f(750, 600));
	//rect4.setFillColor(Color::Black);
	rect4.setTexture(&brickTexture);
	rect4.setTextureRect({ 0,0,2500,1100 });
	walls.push_back(rect4);
	updateMapMatrix(mapMatrix, rect4.getPosition(), rect4.getPosition() + rect4.getSize(), -1);

	RectangleShape rect5;
	rect5.setSize(Vector2f(220, 90));
	rect5.setPosition(Vector2f(1000, 320));
	//rect5.setFillColor(Color::Black);
	rect5.setTexture(&brickTexture);
	rect5.setTextureRect({ 0,0,2200,900 });
	walls.push_back(rect5);
	updateMapMatrix(mapMatrix, rect5.getPosition(), rect5.getPosition() + rect5.getSize(), -1);

	Player player(Vector2f(32, 40), Vector2f(windowWidth / 2.f, windowHeight / 2.f));

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

	player.setMoveAnimation(flandreTexture, 0.1f);
	player.setAttackAnimation(flandreTexture, 0.15f);
	player.setRangeAnimation(flandreTexture, 0.1f);
	player.setRangeAnimation2(daiyouseiTexture, 0.1f);
	player.getSprite()->setPosition(player.getPosition());

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

		if (player.getCurrentHP() <= 0)
		{
			AIGameplayBgm.stop();
			GameOverScreen(std::ref(window), "You Lose!");
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
			//Player Movement
			if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
				playerVelocity.x -= 3.f;
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
				playerVelocity.x += 3.f;
			if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
				playerVelocity.y -= 3.f;
			if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
				playerVelocity.y += 3.f;
			if (magnitude(playerVelocity) > 0)
				player.move(playerVelocity, walls);

			//check for attack input
			if (Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Numpad0))
			{
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				player.rangeAttack(mouseV);
			}
			if (Keyboard::isKeyPressed(Keyboard::Z))
			{
				player.shootSpiral();
			}
			if (Keyboard::isKeyPressed(Keyboard::X))
			{
				Vector2f mouseV = window.mapPixelToCoords(Mouse::getPosition(window));
				player.shootExpand(mouseV);
			}
		}

		player.updateAnimation();
		player.updatePosition();

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
				if ((e->ranged() && e->inRange(player.getPosition())) || e->getBounds().intersects(player.getBounds()))
					continue;
				e->enemyPathfinder(mapMatrix, player.getPosition(), workVector);
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
				if (enemyVector[i]->getBounds().intersects(player.getBounds()))
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

				enemyVector[i]->updateAnimation(player);
				enemyVector[i]->updateSpritePosition();
				enemyVector[i]->drawEnemy(window);
			}
		}

		//update enemy projectiles and draw them
		for (const auto &e : enemyVector)
		{
			e->calcProjCollision(player);
			e->updateProjectile();
			e->drawProjectiles(window);
		}

		//handle player projectile collision
		player.calcProjCollision(enemyVector);

		//update player projectiles
		player.updateProjectile();

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
		player.draw(window);

		//draw player projectiles
		player.drawProjectile(window);

		window.display();
	}
	AIGameplayBgm.stop();
}

void startScreen(RenderWindow &window)
{
	BGM.openFromFile("Resources\\Music\\Necrofantasia.ogg");
	BGM.setVolume(10);
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
						//window.setActive(false);
						BGM.stop();
						AIGameplay(std::ref(window));
						BGM.play();
					}
					else if (menu2Text.getGlobalBounds().contains(mousePos))
					{
						//window.setActive(false);
						CreateRoom(std::ref(window));
					}
					else if (menu3Text.getGlobalBounds().contains(mousePos))
					{
						//window.setActive(false);
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
	window.setVerticalSyncEnabled(true);

	//window.setActive(false);
	startScreen(std::ref(window));

	return 0;
}


/*
*FIXED* - ISSUE#1: pressing back from "connecting room" with valid IP results in freeze due to client thread not terminated
*FIXED* (assuming issue was serverThread not ending) - ISSUE#2: game doesnt close properly sometimes
*FIXED* - ISSUE#3: disconnect server after pressing back on createRoom()
*/