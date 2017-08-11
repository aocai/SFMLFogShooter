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
	Animation();
	Animation(Texture&, int, int, int, int, int, float);
	void update();
	void setPosition(Vector2f);
	Sprite* getSprite();
	void resetFrame();
	bool isOver();
	~Animation();
};