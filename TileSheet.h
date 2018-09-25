#pragma once

#include <glm/glm.hpp>

#include "GLTexture.h"

class TileSheet
{
public:

	void init(const GLTexture& texture, const glm::ivec2& tileDims)
	{
		_texture = texture;
		_dims = tileDims;
	}

	glm::vec4 getUVs(int index)
	{
		int xTile = index % _dims.x;
		int yTile = index / _dims.x;

		glm::vec4 uvs;
		uvs.x = xTile / (float)_dims.x;
		uvs.y = xTile / (float)_dims.y;
		uvs.z = 1.0f / _dims.x;
		uvs.w = 1.0f / _dims.y;

		return uvs;
	}

	GLTexture _texture;
	glm::ivec2 _dims;
};

