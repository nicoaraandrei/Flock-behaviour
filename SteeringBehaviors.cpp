#include "SteeringBehaviors.h"

#include <glm/gtx/norm.hpp>
#include <iostream>


glm::vec2 pointToWorldSpace(const glm::vec2& point, const glm::vec2& agentHeading, const glm::vec2& agentSide, const glm::vec2& agentPosition)
{
	glm::vec3 transPoint = glm::vec3(point.x, point.y, 0.0f);
	glm::mat3 transformMat(1.0f);
	//AgentHeading, AgentSide;
	transformMat[0][0] = agentHeading.x;
	transformMat[0][1] = agentHeading.y;
	transformMat[1][0] = agentSide.x;
	transformMat[1][1] = agentSide.y;
	transformMat[2][0] = agentPosition.x;
	transformMat[2][1] = agentPosition.y;
	
	transPoint = transformMat * transPoint;
	transPoint.x += transformMat[2][0];
	transPoint.y += transformMat[2][1];

	return glm::vec2(transPoint.x, transPoint.y);
}

float randomClamped() //return a float number between -1 and 1
{
	return -1 + 2 * ((float)rand()) / RAND_MAX;
}

SteeringBehaviors::SteeringBehaviors(MovingEntity* agent)
	: _agent(agent)
{
	_wanderJitter = 3.0f;
	_wanderDistance = 35.0f;
	_wanderRadius = 35.0f;
}

SteeringBehaviors::~SteeringBehaviors()
{
}

glm::vec2 SteeringBehaviors::seek(glm::vec2 targetPos)
{
	glm::vec2 desiredVelocity = glm::normalize(targetPos - _agent->getPos()) * _agent->getMaxSpeed();
	std::cout << (desiredVelocity - _agent->getVelocity()).x << "  " << (desiredVelocity - _agent->getVelocity()).y << std::endl;

	return (desiredVelocity - _agent->getVelocity());
}

glm::vec2 SteeringBehaviors::flee(glm::vec2 targetPos)
{
	const float panicDistanceSq = 100.0f * 100.0f;
	if (glm::distance2(_agent->getPos(), targetPos) > panicDistanceSq)
	{
		return glm::vec2(0.0f);
	}

	glm::vec2 desiredVelocity = glm::normalize(_agent->getPos() - targetPos) * _agent->getMaxSpeed();

	return (desiredVelocity - _agent->getVelocity());
}

glm::vec2 SteeringBehaviors::arrive(glm::vec2 targetPos, Deceleration deceleration)
{
	glm::vec2 toTarget = targetPos - _agent->getPos();

	//calculate the distance to the target position
	float dist = glm::length(toTarget);

	if (dist > 0.0f)
	{
		const float decelerationTweaker = 10.0f;

		//calculate the speed required to reach the target
		float speed = dist / ((float)deceleration * decelerationTweaker);
		speed = std::min(speed, _agent->getMaxSpeed());

		glm::vec2 desiredVelocity = toTarget * speed / dist;
		//std::cout << (desiredVelocity - _agent->getVelocity()).x << "  "<< (desiredVelocity - _agent->getVelocity()).y << std::endl;
		return (desiredVelocity - _agent->getVelocity());
	}

	//arriveOff();
	return glm::vec2(0.0f);
}

glm::vec2 SteeringBehaviors::pursuit(MovingEntity* evader)
{
	glm::vec2 toEvader = evader->getPos() - _agent->getPos();

	float relativeHeading = glm::dot(_agent->getDirection(), evader->getDirection());

	//check if the target is in front of agent, acos(0.95) = 18 degs
	if ((glm::dot(toEvader, _agent->getDirection()) > 0) && (relativeHeading < -0.95))
	{
		return seek(evader->getPos());
	}

	//if is not in front, predict evader next position
	float lookAheadTime = glm::length(toEvader) / (_agent->getMaxSpeed() + evader->getSpeed());
	return seek(evader->getPos() + evader->getVelocity() * lookAheadTime);
}

glm::vec2 SteeringBehaviors::evade(MovingEntity* pursuer)
{
	glm::vec2 toPursuer = pursuer->getPos() - _agent->getPos();
	
	float lookAheadTime = glm::length(toPursuer) / (_agent->getMaxSpeed() + pursuer->getSpeed());

	return flee(pursuer->getPos() + pursuer->getVelocity() * lookAheadTime);
}

glm::vec2 SteeringBehaviors::offsetPursuit(MovingEntity* leader, glm::vec2 offset)
{
	glm::vec2 worldOffsetPos = pointToWorldSpace(offset, leader->getDirection(), leader->getSide(), leader->getPos());

	glm::vec2 toOffset = worldOffsetPos - _agent->getPos();

	float lookAheadTime = glm::length(toOffset) / (_agent->getMaxSpeed() + leader->getSpeed());

	return arrive(worldOffsetPos + leader->getVelocity() * lookAheadTime, Deceleration::slow);
}

glm::vec2 SteeringBehaviors::wander()
{
	_wanderTarget += glm::vec2(randomClamped() * _wanderJitter, randomClamped() * _wanderJitter);
	_wanderTarget = glm::normalize(_wanderTarget);
	_wanderTarget *= _wanderRadius;

	glm::vec2 targetLocal = _wanderTarget + glm::vec2(_wanderDistance, 0);
	glm::vec2 targetWorld = pointToWorldSpace(targetLocal, _agent->getDirection(), _agent->getSide(), _agent->getPos());

	return targetWorld - _agent->getPos();
}

glm::vec2 SteeringBehaviors::calculate()
{
	_steeringForce = glm::vec2(0.0f);
	if (_arriveOn)
	{
		_steeringForce += arrive(_agent->getTarget(), Deceleration::slow);
		//_steeringForce += seek(_agent->getTarget());
		//return flee(_agent->getTarget());
	}

	if (_pursuitOn && _agent->getTargetEntity())
	{
		_steeringForce += pursuit(_agent->getTargetEntity());
	}

	if (_evadeOn && _agent->getTargetEntity())
	{
		_steeringForce += evade(_agent->getTargetEntity());
	}

	if (_offsetPursuitOn)
	{
		_steeringForce += offsetPursuit(_agent->getTargetEntity(), _offset);
	}

	if (_wanderOn)
	{
		_steeringForce += wander();
	}

	if (glm::length(_steeringForce) > _agent->getMaxForce())
	{
		_steeringForce = glm::normalize(_steeringForce);
		_steeringForce *= _agent->getMaxForce();
	}

	return _steeringForce;
}
