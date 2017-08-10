#include "Animation.h"

Animation::Animation()
{

}

Animation::Animation(Texture &t, int firstFrame, int lastFrame, int countPerRow, int width, int height, float speed)
{
	frame = 0;
	this->speed = speed;

	for (int i = firstFrame; i <= lastFrame; ++i)
	{
		frames.push_back(IntRect(width*(i%countPerRow), height*(int)(i/8), width, height));
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

Animation::~Animation()
{

}