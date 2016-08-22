#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;

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

//param: boundary = 0-3
//		 0 = left plane
//		 1 = right plane
//		 2 = top plane
//		 3 = bottom plane
//return: plane number the intersection was detected in
int boundClipTest(Vector2f position, Vector2f Q, Vector2f &ret)
{
	Vector2f PQ = Q - position;
	Vector2f normal = PQ.y == 0 ? Vector2f(PQ.y, PQ.x * -1) : Vector2f(PQ.y * -1, PQ.x);
	float num = dotProduct(position, normal);

	float result;
	int successful = -1;

	for (unsigned int boundary = 0; boundary < 4; ++boundary)
	{
		switch (boundary)
		{
		case 0:
			result = num / normal.y;
			if (result >= 0 && result <= 720)
			{
				if (dotProduct(Q - position, Vector2f(0, result) - position) >= 0)
				{
					ret = Vector2f(0, result);
					successful = 0;
				}
			}
			break;
		case 1:
			result = (num - normal.x * 1280) / normal.y;
			if (result >= 0 && result <= 720)
			{
				if (dotProduct(Q - position, Vector2f(1280, result) - position) >= 0)
				{
					ret = Vector2f(1280, result);
					successful = 1;
				}
			}
			break;
		case 2:
			result = num / normal.x;
			if (result >= 0 && result <= 1280)
			{
				if (dotProduct(Q - position, Vector2f(result, 0) - position) >= 0)
				{
					ret = Vector2f(result, 0);
					successful = 2;
				}
			}
			break;
		case 3:
			result = (num - normal.y * 720) / normal.x;
			if (result >= 0 && result <= 1280)
			{
				if (dotProduct(Q - position, Vector2f(result, 720) - position) >= 0)
				{
					ret = Vector2f(result, 720);
					successful = 3;
				}
			}
			break;
		default:
			successful = -1;
			break;
		}
		if (successful != -1)
			break;
	}

	return successful;
}

std::vector<ConvexShape> clipLOS(Vector2f position, Vector2f mousePosition, Vector2f leftArm, Vector2f rightArm)
{
	std::vector<ConvexShape> losVect;
	Color fogColor = Color(80, 80, 80, 255);
	ConvexShape los(4);
	los.setPoint(0, position);

	Vector2f result;
	int successful = boundClipTest(position, leftArm, result);

	Vector2f result2;
	int successful2 = boundClipTest(position, rightArm, result2);

	if (successful == 0)
	{
		if (successful2 == 0)
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
		else if (successful2 == 1)
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
		else if (successful2 == 2)
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
		else if (successful2 == 3)
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
	else if (successful == 1)
	{
		if (successful2 == 0)
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
		else if (successful2 == 1)
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
		else if (successful2 == 2)
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
		else if (successful2 == 3)
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
	else if (successful == 2)
	{
		if (successful2 == 0)
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
		else if (successful2 == 1)
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
		else if (successful2 == 2)
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
		else if (successful2 == 3)
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
	else if (successful == 3)
	{
		if (successful2 == 0)
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
		else if (successful2 == 1)
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
		else if (successful2 == 2)
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
		else if (successful2 == 3)
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

ConvexShape clipShadow(RectangleShape wall, Vector2f position)
{
	int maxPoint = 0;
	int minPoint = 0;
	for (unsigned int j = 0; j < 4; ++j)
	{
		int result = -1;
		for (unsigned int k = 0; k < 4; ++k)
		{
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

	ConvexShape shape(4);
	shape.setPoint(0, wall.getPosition() + wall.getPoint(maxPoint));
	shape.setPoint(1, wall.getPosition() + wall.getPoint(minPoint));

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
std::vector<ConvexShape> checkFog(std::vector<RectangleShape> walls, Vector2f position)
{
	std::vector<ConvexShape> fog;
	for (unsigned int i = 0; i < walls.size(); ++i)
	{
		ConvexShape shape = clipShadow(walls[i], position);
		fog.push_back(shape);
	}
	return fog;
}

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;

	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(windowWidth, windowHeight), "FogShooter", Style::Default, settings);
	window.setFramerateLimit(60);

	std::vector<RectangleShape> walls;

	RectangleShape rect1;
	rect1.setSize(Vector2f(80,220));
	rect1.setPosition(Vector2f(120,110));
	rect1.setFillColor(Color::Black);
	walls.push_back(rect1);

	RectangleShape rect2;
	rect2.setSize(Vector2f(80, 300));
	rect2.setPosition(Vector2f(520, 250));
	rect2.setFillColor(Color::Black);
	walls.push_back(rect2);

	RectangleShape rect3;
	rect3.setSize(Vector2f(60, 160));
	rect3.setPosition(Vector2f(300, 50));
	rect3.setFillColor(Color::Black);
	walls.push_back(rect3);

	RectangleShape rect4;
	rect4.setSize(Vector2f(250, 110));
	rect4.setPosition(Vector2f(750, 600));
	rect4.setFillColor(Color::Black);
	walls.push_back(rect4);

	RectangleShape rect5;
	rect5.setSize(Vector2f(220, 90));
	rect5.setPosition(Vector2f(1000, 320));
	rect5.setFillColor(Color::Black);
	walls.push_back(rect5);

	RectangleShape playerBody;
	playerBody.setSize(Vector2f(46,20));
	playerBody.setOrigin(Vector2f(23, 10));
	playerBody.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));
	playerBody.setFillColor(Color::Black);

	CircleShape playerHead(15);
	playerHead.setOrigin(Vector2f(15, 15));
	playerHead.setPosition(Vector2f(playerBody.getPosition().x, playerBody.getPosition().y));
	playerHead.setFillColor(Color::Black);

	Vector2f lastMousePosition = window.mapPixelToCoords(Mouse::getPosition(window));

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

		//if wall is selected
		if (currentWall < walls.size())
		{
			//move wall to new position
			float posX = window.mapPixelToCoords(Mouse::getPosition(window)).x - deltaX;
			float posY = window.mapPixelToCoords(Mouse::getPosition(window)).y - deltaY;

			if (posX < 0)
				posX = 0;
			else if (posX + walls[currentWall].getSize().x > 1280)
				posX = 1280 - walls[currentWall].getSize().x;
			if (posY < 0)
				posY = 0;
			else if (posY + walls[currentWall].getSize().y > 720)
				posY = 720 - walls[currentWall].getSize().y;

			walls[currentWall].setPosition(Vector2f(posX, posY));
		}

		//Check for collision and prevent movement if collision detected
		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			playerBody.move(Vector2f(-3.0f, 0));
			playerHead.move(Vector2f(-3.0f, 0));
			for (unsigned int i = 0; i < walls.size(); ++i)
			{
				if (walls[i].getGlobalBounds().intersects(playerHead.getGlobalBounds()))
				{
					playerBody.move(Vector2f(3.0f, 0));
					playerHead.move(Vector2f(3.0f, 0));
					break;
				}
			}
			if (playerHead.getPosition().x < 0)
			{
				playerBody.move(Vector2f(-playerHead.getPosition().x, 0));
				playerHead.move(Vector2f(-playerHead.getPosition().x, 0));
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
					playerBody.move(Vector2f(-3.0f, 0));
					playerHead.move(Vector2f(-3.0f, 0));
					break;
				}
			}
			if (playerHead.getPosition().x + 2 * playerHead.getRadius() > 1280)
			{
				playerBody.move(Vector2f(1280 - 2 * playerHead.getRadius() - playerHead.getPosition().x, 0));
				playerHead.move(Vector2f(1280 - 2 * playerHead.getRadius() - playerHead.getPosition().x, 0));
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
					playerBody.move(Vector2f(0, 3.0f));
					playerHead.move(Vector2f(0, 3.0f));
					break;
				}
			}
			if (playerHead.getPosition().y < 0)
			{
				playerBody.move(Vector2f(0, -playerHead.getPosition().y));
				playerHead.move(Vector2f(0, -playerHead.getPosition().y));
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
					playerBody.move(Vector2f(0, -3.0f));
					playerHead.move(Vector2f(0, -3.0f));
					break;
				}
			}
			if (playerHead.getPosition().y + 2 * playerHead.getRadius() > 720)
			{
				playerBody.move(Vector2f(0, 720 - 2 * playerHead.getRadius() - playerHead.getPosition().y));
				playerHead.move(Vector2f(0, 720 - 2 * playerHead.getRadius() - playerHead.getPosition().y));
			}
			if (playerBody.getRotation() != 0)
			{
				playerBody.rotate(-playerBody.getRotation());
			}
		}

		Vector2f mouseCoord = window.mapPixelToCoords(Mouse::getPosition(window));

		//get camera los arms
		Vector2f playerPosition = Vector2f(playerHead.getPosition().x, playerHead.getPosition().y);
		Vector2f PQ;

		if (playerPosition == mouseCoord)
		{
			PQ = playerPosition - lastMousePosition;
		}
		else
		{
			PQ = playerPosition - mouseCoord;
			if (magnitude(PQ) < 30)
			{
				//project onto surface of circle
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
		Vector2f rightArm = Vector2f(PQ.x * cos(15) + PQ.y * (-sin(15)), PQ.x * sin(15) + PQ.y * cos(15)) * 100.0f;
		Vector2f leftArm = Vector2f(PQ.x * cos(-15) + PQ.y * (-sin(-15)), PQ.x * sin(-15) + PQ.y * cos(-15)) * 100.0f;

		//get los
		std::vector<ConvexShape> los = clipLOS(playerPosition, mouseCoord, leftArm, rightArm);

		//check fog
		std::vector<ConvexShape> fog = checkFog(walls, playerPosition);

		//draw background color
		window.clear(Color::White);

		//draw fog and wall
		for (unsigned int i = 0; i < fog.size(); ++i)
		{
				window.draw(fog[i]);
		}

		//draw los
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
		window.draw(playerBody);
		window.draw(playerHead);
		window.display();
	}
	return 0;
}