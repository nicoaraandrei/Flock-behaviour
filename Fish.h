#pragma once

#include <glm/glm.hpp>
#include "SpriteBatch.h"

class Fish
{
public:
	Fish() {}
	Fish(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime, GLuint textureID);
	~Fish();

	void init(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime, GLuint textureID);

	void draw(SpriteBatch& spriteBatch);
	//return true when the life time is zero
	bool update(float deltaTime);

	glm::vec2 getPos();
	void setDirection(glm::vec2 direction);

private:
	float _speed;
	glm::vec2 _direction;
	glm::vec2 _position;
	int _lifeTime;
	GLuint m_textureID;
};

