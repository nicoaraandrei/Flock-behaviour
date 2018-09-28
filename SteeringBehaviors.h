#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "MovingEntity.h"
#include "StaticEntity.h"

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
	void setWalls(std::vector<StaticEntity> *walls) { _walls = walls; }
	void createFeelers();

	bool accumulateForce(glm::vec2 &totalForce, glm::vec2 forceToAdd);
	glm::vec2 calculate();

	glm::vec2 seek(const glm::vec2 targetPos);
	glm::vec2 flee(const glm::vec2 targetPos);
	glm::vec2 arrive(const glm::vec2 targetPos, Deceleration deceleration);
	glm::vec2 pursuit(const MovingEntity* evader);
	glm::vec2 evade(const MovingEntity* pursuer);
	glm::vec2 offsetPursuit(const MovingEntity* leader, const glm::vec2 offset);
	glm::vec2 wander();
	glm::vec2 wallAvoidance();

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

	bool isWanderOn() { return _wanderOn == true; }
	
private:
	MovingEntity* _agent;
	glm::vec2 _steeringForce;
	glm::vec2 _offset;
	glm::vec2 _wanderTarget;
	std::vector<glm::vec2> _feelers;
	std::vector<StaticEntity> *_walls;

	float _wanderRadius;
	float _wanderDistance;
	float _wanderJitter;
	float _wallDetectionFeelerLength;
	
	bool _arriveOn = false;
	bool _pursuitOn = false;
	bool _evadeOn = false;
	bool _offsetPursuitOn = false;
	bool _wanderOn = false;
};

