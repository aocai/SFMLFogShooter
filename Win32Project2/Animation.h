#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Animation
{
private:
	float frame;
	float speed;
	Sprite sprite;
	std::vector<IntRect> frames;
public:
	Animation() = default;
	Animation(const Texture&, int, int, int, int, int, float);
	void update();
	void setPosition(Vector2f);
	void setOrigin(Vector2f);
	void scale(Vector2f);
	void rotate(float);
	void move(Vector2f);

	const Sprite* getSprite() const;
	void resetFrame();
	bool isOver() const;
};