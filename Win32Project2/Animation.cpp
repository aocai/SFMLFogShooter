#include "Animation.h"

Animation::Animation()
{

}

Animation::Animation(Texture &t, int x, int y, int count, int width, int height, float speed)
{
	frame = 0;
	this->speed = speed;

	for (int i = 0; i < count; ++i)
	{
		frames.push_back(IntRect((x + i*width)%256, y+floor((x+i*width)/256)*height, width, height));
	}
	
	sprite.setTexture(t);
	sprite.setTextureRect(frames.front());
}

void Animation::update()
{
	frame += speed;
	if (frame >= frames.size())
	{
		frame -= frames.size();
	}
	sprite.setTextureRect(frames[(int)frame]);
}

void Animation::resetFrame()
{
	frame = 0;
}

void Animation::setPosition(Vector2f p)
{
	sprite.setPosition(p);
}

Sprite* Animation::getSprite()
{
	return &sprite;
}

bool Animation::isOver()
{
	return (frame + speed) >= frames.size();
}