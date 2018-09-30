#include "MovingEntity.h"
#include "ResourceManager.h"

#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

MovingEntity::MovingEntity(glm::vec2 pos, float rotation, glm::vec2 velocity, float mass, float maxForce, float maxSpeed, float maxTurnRate, float scale, const std::string& texturePath) : 
	BaseEntity(0, pos, scale), _velocity(velocity), _mass(mass), _maxSpeed(maxSpeed), _direction(glm::vec2(sin(rotation), -cos(rotation))), _maxTurnRate(maxTurnRate), _maxForce(maxForce)
{
	_side = glm::vec2(-_direction.y, _direction.x);
	_target = glm::vec2(0.0f, 0.0f);

	_scale = glm::vec2(scale, scale);

	_steering = new SteeringBehaviors(this);


	GLTexture texture = ResourceManager::getTexture(texturePath);

	_texture.init(texture, glm::ivec2(3,1));
}


MovingEntity::~MovingEntity()
{

}

void MovingEntity::init(glm::vec2 pos, float rotation, glm::vec2 velocity, float mass, float maxForce, float maxSpeed, float maxTurnRate, float scale, const std::string& texturePath)
{
	setEntityType(0);
	_position = pos;
	_boundingRadius = scale;
	_velocity = velocity;
	_mass = mass;
	_maxSpeed = maxSpeed;
	_direction = glm::vec2(sin(rotation), -cos(rotation));
	_maxTurnRate = maxTurnRate;
	_maxForce = maxForce;
	_side = glm::vec2(-_direction.y, _direction.x);
	_target = glm::vec2(0.0f);

	_scale = glm::vec2(scale, scale);

	_steering = new SteeringBehaviors(this);

	GLTexture texture = ResourceManager::getTexture(texturePath);

	_texture.init(texture, glm::ivec2(3, 1));
}




void MovingEntity::draw(SpriteBatch& spriteBatch)
{
	//glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	ColorRGBA8 whiteColor(255, 255, 255, 255);

	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, _scale.x, _scale.y);


	int tileIndex = 0;
	int numTiles = 3;


	tileIndex += (int)_animTime % numTiles;

	spriteBatch.draw(posAndSize, _texture.getUVs(tileIndex), _texture._texture.id, 0.0f, whiteColor, _direction);
}

void MovingEntity::update(float deltaTime)
{
	glm::vec2 steeringForce = _steering->calculate();
	
	glm::vec2 acceleration = steeringForce / _mass;

	_velocity += acceleration * deltaTime;

	if (glm::length(_velocity) > _maxSpeed)
	{
		_velocity = glm::normalize(_velocity);
		_velocity *= _maxSpeed;
	}


	_position += _velocity * deltaTime;

	if (glm::length2(_velocity) > 0.000001f)
	{
		_direction = glm::normalize(_velocity);

		_side = glm::vec2(-_direction.y, _direction.x);
	}

	_animTime += glm::length(_velocity)/30.0f * deltaTime;
}


bool MovingEntity::rotateDirectionToFacePosition(glm::vec2 target)
{
	glm::vec2 toTarget = glm::normalize(target - _position);

	//angle between the direction vector and target
	float angle = acos(glm::dot(_direction, toTarget));
	//if the entity is already facing the target
	if (angle < 0.00001)
	{
		return true;
	}

	//clamp the amount to turn(angle) to the max turn rate
	if (angle > _maxTurnRate)
	{
		angle = _maxTurnRate;
	}

	if (_direction.y*toTarget.x > _direction.x*toTarget.y)
	{
		angle *= -1;
	}

	glm::rotate(_direction, angle);
	glm::rotate(_velocity, angle);

	_side = glm::vec2(-_direction.y, _direction.x);

	return false;
}

void MovingEntity::setDirection(glm::vec2 direction)
{
	//terminate the program if the new direction has zero length
	assert((glm::length2(direction) - 1.0f) < 0.00001);

	_direction = direction;

	_side = glm::vec2(-_direction.y, _direction.x);
}