#pragma once
#include "BaseEntity.h"
#include "GLTexture.h"
#include "SpriteBatch.h"

#include <string>

class StaticEntity :
	public BaseEntity
{
public:
	StaticEntity(glm::vec2 pos, glm::vec2 scale, const std::string& texturePath, glm::vec4 uv = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	StaticEntity() {}
	~StaticEntity();

	void calculateNormals(bool inverse = false);
	void init(glm::vec2 pos, glm::vec2 scale, const std::string& texturePath, glm::vec4 uv = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	void draw(SpriteBatch& spriteBatch);

	glm::vec2 getNormals() { return _normals; }
	glm::vec2 _normals;

private:

	GLTexture _texture;
	glm::vec4 _uv;
};
