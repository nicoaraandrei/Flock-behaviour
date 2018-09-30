#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	void init(int screenWidth, int screenHeight);

	void update();

	void setPosition(const glm::vec2& newPosition)
	{
		_position = newPosition;
		_needsMatrixUpdate = true;
	}

	void setScale(float newScale)
	{
		if (newScale < 0.1f)
		{
			_scale = 0.1f;
		}
		else if (newScale > 4.0f)
		{
			_scale = 4.0f;
		}
		else
		{
			_scale = newScale;
		}
		_needsMatrixUpdate = true;

	}

	glm::vec2 getPosition() { return _position; }

	float getScale() { return _scale; }

	glm::mat4 getCameraMatrix() { return _cameraMatrix; }

	glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);

private:
	int _screenWidth, _screenHeight;
	bool _needsMatrixUpdate;
	float _scale;
	glm::vec2 _position;
	// view-projection matrix
	glm::mat4 _cameraMatrix;
	// projection matrix
	glm::mat4 _orthoMatrix;
	float _minScale, _maxScale;


};
