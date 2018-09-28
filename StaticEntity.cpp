#include "StaticEntity.h"
#include "ResourceManager.h"


void StaticEntity::calculateNormals(bool inverse)
{
	glm::vec2 temp = _scale;
	if (temp.x > temp.y)
	{
		temp.y = 0.0f;
	}
	else
	{
		temp.x = 0.0f;
	}
	temp = glm::normalize(temp);
	_normals = glm::vec2(-temp.y, temp.x);
	if (inverse)
	{
		_normals = glm::vec2(temp.y, -temp.x);
	}
}

StaticEntity::StaticEntity(glm::vec2 pos, glm::vec2 scale, const std::string& texturePath, glm::vec4 uv)
	: BaseEntity(1, pos, scale), _uv(uv)
{
	_texture = ResourceManager::getTexture(texturePath);
	calculateNormals();
}

void StaticEntity::init(glm::vec2 pos, glm::vec2 scale, const std::string& texturePath, glm::vec4 uv)
{
	_position = pos;
	_scale = scale;
	_uv = uv;
	_texture = ResourceManager::getTexture(texturePath);
	calculateNormals();
}


StaticEntity::~StaticEntity()
{
}

void StaticEntity::draw(SpriteBatch& spriteBatch)
{
	ColorRGBA8 whiteColor(255, 255, 255, 255);

	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, _scale.x, _scale.y);

	spriteBatch.draw(posAndSize, _uv, _texture.id, 1.0f, whiteColor);
}
