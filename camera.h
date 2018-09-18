#ifndef CAMERA_H
#define CAMERA_H

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
		_scale = newScale;
		_needsMatrixUpdate = true;

	}

	glm::vec2 getPosition() { return _position; }

	float getScale() { return _scale; }

	glm::mat4 getCameraMatrix() { return _cameraMatrix; }

private:
	int _screenWidth, _screenHeight;
	bool _needsMatrixUpdate;
	float _scale;
	glm::vec2 _position;
	// view-projection matrix
	glm::mat4 _cameraMatrix;
	// projection matrix
	glm::mat4 _orthoMatrix;


};
#endif