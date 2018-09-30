#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "MovingEntity.h"
#include "StaticEntity.h"

class MovingEntity;

enum SteeringStates
{
	seek = 0,
	flee = 1,
	arrive = 2,
	pursuit = 3,
	evade = 4,
	wander = 5,
	wallAvoidance = 6,
	offsetPursuit = 7,
	separation = 8,
	alignment = 9,
	cohesion = 10
};

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
	void setNeighbors(std::vector<MovingEntity> *neighbors) { _neighbors = neighbors; }
	void createFeelers();
	void tagNeighbors(float radius);

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
	glm::vec2 separation();
	glm::vec2 alignment();
	glm::vec2 cohesion();
	void noOverlap();

	bool states[11] = { false };

private:

	MovingEntity* _agent;
	glm::vec2 _steeringForce;
	glm::vec2 _offset;
	glm::vec2 _wanderTarget;
	std::vector<glm::vec2> _feelers;
	std::vector<StaticEntity> *_walls;
	std::vector<MovingEntity> *_neighbors;

	float _wanderRadius;
	float _wanderDistance;
	float _wanderJitter;
	float _wallDetectionFeelerLength;
	
	float weights[11];

};

