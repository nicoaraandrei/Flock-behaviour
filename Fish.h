#pragma once

#include <glm/glm.hpp>
#include "SpriteBatch.h"

class Fish
{
public:
	Fish(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime);
	~Fish();

	void draw(SpriteBatch& spriteBatch);
	//return true when the life time is zero
	bool update(float deltaTime);

private:
	float _speed;
	glm::vec2 _direction;
	glm::vec2 _position;
	int _lifeTime;
};

