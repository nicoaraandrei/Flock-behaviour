#pragma once

#include <glm/glm.hpp>
#include "MovingEntity.h"

class MovingEntity;

enum Deceleration
{
	slow = 3,
	normal = 2,
	fast = 1
};

class SteeringBehaviors
{
public:
	SteeringBehaviors(MovingEntity* agent);
	~SteeringBehaviors();

	void setAgent(MovingEntity* agent) { _agent = agent; }
	void setOffset(glm::vec2 offset) { _offset = offset; }


	glm::vec2 calculate();

	glm::vec2 seek(glm::vec2 targetPos);
	glm::vec2 flee(glm::vec2 targetPos);
	glm::vec2 arrive(glm::vec2 targetPos, Deceleration deceleration);
	glm::vec2 pursuit(MovingEntity* evader);
	glm::vec2 evade(MovingEntity* pursuer);
	glm::vec2 offsetPursuit(MovingEntity* leader, glm::vec2 offset);
	glm::vec2 wander();

	void arriveOn() { _arriveOn = true; }
	void arriveOff() { _arriveOn = false; }
	void pursuitOn() { _pursuitOn = true; }
	void pursuitOff() { _pursuitOn = false; }
	void evadeOn() { _evadeOn = true; }
	void evadeOff() { _evadeOn = false; }
	void offsetPursuitOn() { _offsetPursuitOn = true; }
	void offsetPursuitOff() { _offsetPursuitOn = false; }
	void wanderOn() { _wanderOn = true; }
	void wanderOff() { _wanderOn = false; }
	
private:
	MovingEntity* _agent;
	glm::vec2 _steeringForce;
	glm::vec2 _offset;
	glm::vec2 _wanderTarget;

	float _wanderRadius;
	float _wanderDistance;
	float _wanderJitter;
	
	bool _arriveOn = false;
	bool _pursuitOn = false;
	bool _evadeOn = false;
	bool _offsetPursuitOn = false;
	bool _wanderOn = false;
};

