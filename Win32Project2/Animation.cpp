#include "Animation.h"
#include <iostream>

Animation::Animation(const Texture &t, int x, int y, int count, int width, int height, float speed)
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
	if (frames.empty())
		return;

	frame += speed;
	if (frame >= frames.size())
	{
		frame -= frames.size();
	}
	sprite.setTextureRect(frames[static_cast<int>(frame)]);
}

void Animation::resetFrame()
{
	frame = 0;
}

void Animation::setPosition(Vector2f position)
{
	sprite.setPosition(position);
}

void Animation::setOrigin(Vector2f origin)
{
	sprite.setOrigin(origin);
}

void Animation::scale(Vector2f scale)
{
	sprite.scale(scale);
}

void Animation::rotate(float angle)
{
	sprite.rotate(angle);
}

void Animation::move(Vector2f v)
{
	sprite.move(v);
}

const Sprite* Animation::getSprite() const
{
	return &sprite;
}

bool Animation::isOver() const
{
	return (frame + speed) >= frames.size();
}