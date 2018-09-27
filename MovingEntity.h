#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "SpriteBatch.h"
#include "TileSheet.h"
#include "BaseEntity.h"
#include "SteeringBehaviors.h"

class SteeringBehaviors;

class MovingEntity : public BaseEntity
{
public:
	MovingEntity() : BaseEntity(0) {}
	MovingEntity(glm::vec2 pos, float rotation, glm::vec2 velocity, float mass, float maxForce, float maxSpeed, float maxTurnRate, float scale, const std::string& texturePath);
	~MovingEntity();

	void init(glm::vec2 pos, float rotation, glm::vec2 velocity, float mass, float maxForce, float maxSpeed, float maxTurnRate, float scale, const std::string& texturePath);

	void draw(SpriteBatch& spriteBatch);
	//return true when the life time is zero
	void update(float deltaTime);

	glm::vec2 getVelocity()const { return _velocity; }
	void setVelocity(glm::vec2 newVel) { _velocity = newVel; }

	float getMass()const { return _mass; }

	glm::vec2 getSide()const { return _side; }

	float getMaxSpeed()const { return _maxSpeed; }
	void setMaxSpeed(float newSpeed) { _maxSpeed = newSpeed; }

	float getMaxForce()const { return _maxForce; }
	void setMaxForce(float newForce) { _maxForce = newForce; }

	bool isSpeedMaxedOut() const { return _maxSpeed * _maxSpeed >= glm::length2(_velocity); }
	float getSpeed()const { return _velocity.length(); }
	float getSpeedSq()const { return glm::length2(_velocity); }

	glm::vec2 getDirection()const { return _direction; }
	void setDirection(glm::vec2 direction);
	bool rotateDirectionToFacePosition(glm::vec2 target);

	float getMaxTurnRate()const { return _maxTurnRate; }
	void setMaxTurnRate(float val) { _maxTurnRate = val; }

	void setTarget(glm::vec2 target) { _target = target; }
	glm::vec2 getTarget()const { return _target; }

	void setTargetEntity(MovingEntity* targetEntity) { _targetEntity = targetEntity; }
	MovingEntity* getTargetEntity()const { return _targetEntity; }

	SteeringBehaviors* getSteering()const { return _steering; }

private:
	//float _speed;
	glm::vec2 _velocity;
	glm::vec2 _direction;
	//perpendicular with the direction vector
	glm::vec2 _side;
	glm::vec2 _target;
	MovingEntity* _targetEntity;

	float _mass;
	float _maxSpeed;
	//maximum force this entity can produce to move itself
	float _maxForce;
	//maximum rate(radians per second) can this entity rotate
	float _maxTurnRate;

	SteeringBehaviors* _steering;

	TileSheet _texture;
	float _animTime = 0.0f;
	float _animSpeed = 0.1f;
};

