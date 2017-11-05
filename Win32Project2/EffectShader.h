#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class EffectShader : public sf::Drawable
{
private:
	std::string effectName;
	bool shaderAvailable;
	virtual bool onLoad() = 0;
public:
	virtual ~EffectShader()
	{
	}

	const std::string& getName() const
	{
		return effectName;
	}

	void load()
	{
		shaderAvailable = sf::Shader::isAvailable() && onLoad();
	}

	void update(float time, float posX, float posY)
	{

	}
};