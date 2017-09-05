#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <stack>
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
sf::TcpSocket socket;


void server()
{
	std::cout << "server running..." << std::endl;
	sf::TcpListener listener;
	std::cout << "accepting socket on port " << PORT << std::endl;
	listener.listen(PORT);
	listener.accept(socket);
	std::cout << "socket accepted on ip " << socket.getRemoteAddress().toString() << std::endl;
}

bool client()
{
	auto ip = sf::IpAddress::getLocalAddress();
	std::cout << ip.toString() << std::endl;
	auto result = socket.connect(ip, PORT);
	std::cout << result << std::endl;
	if (result == sf::Socket::Done)
	{
		std::cout << "ran" << std::endl;
		return true;
	}
	std::cout << "did not run" << std::endl;
	return false;
}

struct myPacket
{
	Vector2f movement;
};


Vector2f p2Movement{ 0, 0 };

void receivemsg()
{
	while (true)
	{
		sf::Packet packet;
		std::cout << "receiving packet" << std::endl;
		socket.receive(packet);
		if (packet.endOfPacket())
		{
			std::cout << "empty packet!" << std::endl;
		}
		else
		{
			const myPacket *p = static_cast<const myPacket*>(packet.getData());
			p2Movement = p->movement;
		}
	}
}


void sendSocketMessage(Vector2f message)
{
	sf::Packet packet;
	myPacket p{ message };
	packet.append(&p, sizeof(myPacket));
	
	while (socket.send(packet) == sf::Socket::Partial);
}

void PlayerGameplay(RenderWindow &window)
{
	Texture ayaTexture;
	Texture cirnoTexture;
	Texture flandreTexture;
	Texture sakuyaTexture;
	Texture suikaTexture;
	Texture daiyouseiTexture;
	ayaTexture.loadFromFile("sprites\\Aya Shameimaru.png");
	cirnoTexture.loadFromFile("sprites\\Cirno.png");
	flandreTexture.loadFromFile("sprites\\Flandre Scarlet.png");
	sakuyaTexture.loadFromFile("sprites\\Sakuya Izayoi.png");
	suikaTexture.loadFromFile("sprites\\Suika Ibuki.png");
	daiyouseiTexture.loadFromFile("sprites\\Daiyousei.png");

	Player player(Vector2f(32, 40), Vector2f(200, windowHeight / 2.f));
	player.setMoveAnimation(flandreTexture, 0.1f);
	player.setAttackAnimation(flandreTexture, 0.15f);
	player.setRangeAnimation(flandreTexture, 0.1f);
	player.setRangeAnimation2(daiyouseiTexture, 0.1f);
	player.getSprite()->setPosition(player.getPosition());

	Player player2(Vector2f(32, 40), Vector2f(1000, windowHeight / 2.f));
	player2.setMoveAnimation(flandreTexture, 0.1f);
	player2.setAttackAnimation(flandreTexture, 0.15f);
	player2.setRangeAnimation(flandreTexture, 0.1f);
	player2.setRangeAnimation2(daiyouseiTexture, 0.1f);
	player2.getSprite()->setPosition(player2.getPosition());
	
	
	std::vector<RectangleShape> walls;

	while (window.isOpen())
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
			player.move(playerVelocity, walls);
			sendSocketMessage(playerVelocity);
		}

		player2.move(p2Movement, walls);

		player.updateAnimation();
		player.updatePosition();

		player2.updateAnimation();
		player2.updatePosition();

		window.clear(Color::White);

		player.draw(window);
		player2.draw(window);
		window.display();
	}
}


void ConnectionWaiting(RenderWindow &window)
{
	auto serverThread = new sf::Thread(&server);
	serverThread->launch();

	auto receiveThread = new sf::Thread(&receivemsg);
	receiveThread->launch();

	sf::Font font;
	font.loadFromFile("LLPIXEL3.ttf");

	sf::Text connectingText;
	connectingText.setString("Waiting for connection...");
	connectingText.setColor(Color::Black);
	connectingText.setFont(font);
	connectingText.setOrigin(Vector2f(connectingText.getGlobalBounds().width / 2.f, connectingText.getGlobalBounds().height / 2.f));
	connectingText.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));


	window.clear(Color::White);
	window.draw(connectingText);
	window.display();
	serverThread->wait();
	PlayerGameplay(std::ref(window));
}


void AIGameplay(RenderWindow &window)
{
	std::vector<double> mapMatrix((windowWidth / 10) * (windowHeight / 10), 1);
	std::vector<int> workVector((windowWidth / 10) * (windowHeight / 10), 0);

	Clock clock;
	float pathTime = clock.getElapsedTime().asSeconds();

	std::vector<RectangleShape> walls;

	//create the walls
	RectangleShape rect1;
	rect1.setSize(Vector2f(80, 220));
	rect1.setPosition(Vector2f(120, 110));
	rect1.setFillColor(Color::Black);
	walls.push_back(rect1);
	updateMapMatrix(mapMatrix, rect1.getPosition(), rect1.getPosition() + rect1.getSize(), -1);

	RectangleShape rect2;
	rect2.setSize(Vector2f(80, 300));
	rect2.setPosition(Vector2f(520, 250));
	rect2.setFillColor(Color::Black);
	walls.push_back(rect2);
	updateMapMatrix(mapMatrix, rect2.getPosition(), rect2.getPosition() + rect2.getSize(), -1);

	RectangleShape rect3;
	rect3.setSize(Vector2f(60, 160));
	rect3.setPosition(Vector2f(300, 50));
	rect3.setFillColor(Color::Black);
	walls.push_back(rect3);
	updateMapMatrix(mapMatrix, rect3.getPosition(), rect3.getPosition() + rect3.getSize(), -1);

	RectangleShape rect4;
	rect4.setSize(Vector2f(250, 110));
	rect4.setPosition(Vector2f(750, 600));
	rect4.setFillColor(Color::Black);
	walls.push_back(rect4);
	updateMapMatrix(mapMatrix, rect4.getPosition(), rect4.getPosition() + rect4.getSize(), -1);

	RectangleShape rect5;
	rect5.setSize(Vector2f(220, 90));
	rect5.setPosition(Vector2f(1000, 320));
	rect5.setFillColor(Color::Black);
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
	ayaTexture.loadFromFile("sprites\\Aya Shameimaru.png");
	cirnoTexture.loadFromFile("sprites\\Cirno.png");
	flandreTexture.loadFromFile("sprites\\Flandre Scarlet.png");
	sakuyaTexture.loadFromFile("sprites\\Sakuya Izayoi.png");
	suikaTexture.loadFromFile("sprites\\Suika Ibuki.png");
	daiyouseiTexture.loadFromFile("sprites\\Daiyousei.png");

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

		//draw background color
		window.clear(Color::White);

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

		if (player.getCurrentHP() <= 0)
			window.close();

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

	RectangleShape menu1;
	menu1.setSize(Vector2f(200, 50));
	menu1.setOrigin(menu1.getSize() / 2.f);
	menu1.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f));
	menu1.setFillColor(Color::White);

	sf::Font font;
	font.loadFromFile("LLPIXEL3.ttf");

	sf::Text menu1Text;
	menu1Text.setString("VS AI");
	menu1Text.setColor(Color::Black);
	menu1Text.setFont(font);
	menu1Text.setOrigin(Vector2f(menu1Text.getGlobalBounds().width / 2.f, menu1Text.getGlobalBounds().height / 2.f));
	menu1Text.setPosition(menu1.getPosition());

	RectangleShape menu2;
	menu2.setSize(Vector2f(200, 50));
	menu2.setOrigin(menu2.getSize() / 2.f);
	menu2.setPosition(Vector2f(windowWidth / 2.f, windowHeight / 2.f) + 2.f * Vector2f(0, menu1.getSize().y));
	menu2.setFillColor(Color::White);

	sf::Text menu2Text;
	menu2Text.setString("VS Player");
	menu2Text.setColor(Color::Black);
	menu2Text.setFont(font);
	menu2Text.setOrigin(Vector2f(menu2Text.getGlobalBounds().width / 2.f, menu2Text.getGlobalBounds().height / 2.f));
	menu2Text.setPosition(menu2.getPosition());

	while (window.isOpen())
	{
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			auto mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
			if (menu1.getGlobalBounds().contains(mousePos))
			{
				window.setActive(false);
				AIGameplay(std::ref(window));
				break;
			}
			else if (menu2.getGlobalBounds().contains(mousePos))
			{
				window.setActive(false);
				ConnectionWaiting(std::ref(window));
				break;
			}
		}
		window.draw(menu1);
		window.draw(menu1Text);
		window.draw(menu2);
		window.draw(menu2Text);
		window.display();
	}

	return 0;
}