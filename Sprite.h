#pragma once
#include <glew.h>
#include <string>

#include "GLTexture.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	void init(float x, float y, float width, float height, std::string texturePath);

	void draw();

private:
	float _x;
	float _y;
	float _width;
	float _height;
	GLuint _vboID;
	GLuint _vaoID;
	GLTexture _texture;
};

