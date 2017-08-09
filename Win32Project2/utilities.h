#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

float determinant(Vector2f v1, Vector2f v2);

float dotProduct(Vector2f v1, Vector2f v2);

float magnitude(Vector2f v);

void updateMapMatrix(std::vector<double> &mapMatrix, Vector2f topLeft, Vector2f bottomRight, int value);

Vector2f rotateVector2f(Vector2f v, double angle);