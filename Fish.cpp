#include "Fish.h"
#include "ResourceManager.h"


Fish::Fish(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime, const std::string& texturePath)
{
	init(pos, dir, speed, lifeTime, texturePath);
}


Fish::~Fish()
{
}

void Fish::init(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime, const std::string& texturePath)
{
	_position = pos;
	_direction = dir;
	_speed = speed;
	_lifeTime = lifeTime;

	GLTexture texture = ResourceManager::getTexture(texturePath);

	_texture.init(texture, glm::ivec2(3,1));
}


void Fish::draw(SpriteBatch& spriteBatch)
{
	//glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	ColorRGBA8 whiteColor(255, 255, 255, 255);

	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, 30, 30);


	int tileIndex = 0;
	int numTiles = 3;


	tileIndex += (int)_animTime % numTiles;

	spriteBatch.draw(posAndSize, _texture.getUVs(tileIndex), _texture._texture.id, 0.0f, whiteColor, _direction);
}

bool Fish::update(float deltaTime)
{
	_position += _direction * _speed * deltaTime;
	_lifeTime--;
	_animTime += _animSpeed * deltaTime;

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