#include "Fish.h"
#include "ResourceManager.h"


Fish::Fish(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime, GLuint textureID)
{
	init(pos, dir, speed, lifeTime, textureID);
}


Fish::~Fish()
{
}

void Fish::init(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime, GLuint textureID)
{
	_position = pos;
	_direction = dir;
	_speed = speed;
	_lifeTime = lifeTime;
	m_textureID = textureID;
}


void Fish::draw(SpriteBatch& spriteBatch)
{
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	ColorRGBA8 whiteColor(255, 255, 255, 255);

	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, 30, 30);

	spriteBatch.draw(posAndSize, uv, m_textureID, 0.0f, whiteColor, _direction);
}

bool Fish::update(float deltaTime)
{
	_position += _direction * _speed * deltaTime;
	_lifeTime--;
	if (_lifeTime == 0)
	{
		return true;
	}
	return false;
}

glm::vec2 Fish::getPos()
{
	return _position;
}

void Fish::setDirection(glm::vec2 direction)
{
	_direction = direction;
}