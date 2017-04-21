#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;

extern RectangleShape playerBody;
extern CircleShape playerHead;
extern std::vector<std::shared_ptr<CircleProjectile>> *allCircleProjectiles;
extern std::vector<double> *mapMatrix; //1 = valid path, -1 = non-valid, 20 screen pixels per matrix unit

float determinant(Vector2f v1, Vector2f v2);
float dotProduct(Vector2f v1, Vector2f v2);
float magnitude(Vector2f v);