#include "Fish.h"
#include "ResourceManager.h"

Fish::Fish(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime)
{
	_position = pos;
	_direction = dir;
	_speed = speed;
	_lifeTime = lifeTime;
}


Fish::~Fish()
{
}


void Fish::draw(SpriteBatch& spriteBatch)
{
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static GLTexture redFishTexture = ResourceManager::getTexture("Textures/Fish/red_fish_1.png");
	ColorRGBA8 whiteColor(255, 255, 255, 255);

	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, 30, 30);

	spriteBatch.draw(posAndSize, uv, redFishTexture.id, 0.0f, whiteColor);
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
