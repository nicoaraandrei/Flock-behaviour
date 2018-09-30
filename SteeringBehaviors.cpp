#include "SteeringBehaviors.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <limits>

#define PI 3.14159

bool LineIntersection2D(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D,	float& dist, glm::vec2&  point)
{

	float rTop = (A.y - C.y)*(D.x - C.x) - (A.x - C.x)*(D.y - C.y);
	float rBot = (B.x - A.x)*(D.y - C.y) - (B.y - A.y)*(D.x - C.x);

	float sTop = (A.y - C.y)*(B.x - A.x) - (A.x - C.x)*(B.y - A.y);
	float sBot = (B.x - A.x)*(D.y - C.y) - (B.y - A.y)*(D.x - C.x);

	if ((rBot == 0) || (sBot == 0))
	{
		//lines are parallel
		return false;
	}

	float r = rTop / rBot;
	float s = sTop / sBot;

	if ((r > 0) && (r < 1) && (s > 0) && (s < 1))
	{
		dist = glm::distance(A, B) * r;

		point = A + r * (B - A);

		return true;
	}

	else
	{
		dist = 0;

		return false;
	}
}

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
	_wanderJitter = 40.0f;
	_wanderDistance = 3.5f;
	_wanderRadius = 2.0f;

	_wallDetectionFeelerLength = 40.0f;
	_feelers.resize(3);

	for (int i = 0; i < 11; i++)
	{
		weights[i] = 1.0f;
	}

	weights[SteeringStates::wallAvoidance] = 10.0f;
	weights[SteeringStates::evade] = 0.1f;
	weights[SteeringStates::cohesion] = 1.0f;

	states[SteeringStates::wallAvoidance] = true;
}

SteeringBehaviors::~SteeringBehaviors()
{
}

glm::vec2 SteeringBehaviors::seek(const glm::vec2 targetPos)
{
	glm::vec2 desiredVelocity = glm::normalize(targetPos - _agent->getPos()) * _agent->getMaxSpeed();

	return (desiredVelocity - _agent->getVelocity());
}

glm::vec2 SteeringBehaviors::flee(const glm::vec2 targetPos)
{
	const float panicDistanceSq = 100.0f * 100.0f;
	if (glm::distance2(_agent->getPos(), targetPos) > panicDistanceSq)
	{
		return glm::vec2(0.0f);
	}

	glm::vec2 desiredVelocity = glm::normalize(_agent->getPos() - targetPos) * _agent->getMaxSpeed();

	return (desiredVelocity - _agent->getVelocity());
}

glm::vec2 SteeringBehaviors::arrive(const glm::vec2 targetPos, Deceleration deceleration)
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
		return (desiredVelocity - _agent->getVelocity());
	}

	//arriveOff();
	return glm::vec2(0.0f);
}

glm::vec2 SteeringBehaviors::pursuit(const MovingEntity* evader)
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

glm::vec2 SteeringBehaviors::evade(const MovingEntity* pursuer)
{
	glm::vec2 toPursuer = pursuer->getPos() - _agent->getPos();

	float lookAheadTime = glm::length(toPursuer) / (_agent->getMaxSpeed() + pursuer->getSpeed());

	return flee(pursuer->getPos() + pursuer->getVelocity() * lookAheadTime);
}

glm::vec2 SteeringBehaviors::offsetPursuit(const MovingEntity* leader, const glm::vec2 offset)
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

void SteeringBehaviors::createFeelers()
{
	//feeler pointing straight in front
	_feelers[0] = _agent->getPos() + _wallDetectionFeelerLength * _agent->getDirection();

	//feeler to the left
	float angle = PI / 2 * 3.0f;
	_feelers[1] = _agent->getPos() + _wallDetectionFeelerLength / 2.0f * glm::rotate(_agent->getDirection(), angle);

	//feeler to the right
	angle = PI / 2;
	_feelers[2] = _agent->getPos() + _wallDetectionFeelerLength / 2.0f * glm::rotate(_agent->getDirection(), angle);
}

glm::vec2 SteeringBehaviors::wallAvoidance()
{
	createFeelers();

	float distToThisIntersectionPoint = 0.0f;
	float distToClosestIintersectionPoint = (std::numeric_limits<float>::max)();

	int closestWall = -1;

	glm::vec2 steeringForceTemp(0.0f);
	glm::vec2 point(0.0f);
	glm::vec2 closestPoint(0.0f);

	for (int feelerIndex = 0; feelerIndex < _feelers.size(); feelerIndex++)
	{
		for (auto  wall = _walls->begin(); wall != _walls->end(); wall++)
		{
			glm::vec2 from = wall->getPos();
			glm::vec2 scale = wall->getScale();
			glm::vec2 to;
			if (scale.x > scale.y)
			{
				to = glm::vec2(from.x + scale.x, from.y);
			}
			else
			{
				to = glm::vec2(from.x, from.y + scale.y);
			}
			if (LineIntersection2D(_agent->getPos(), _feelers[feelerIndex], from, to, distToThisIntersectionPoint, point))
			{
				if (distToThisIntersectionPoint < distToClosestIintersectionPoint)
				{
					distToClosestIintersectionPoint = distToThisIntersectionPoint;
					closestWall = wall - _walls->begin();
					closestPoint = point;
				}
			}
		}

		if (closestWall >= 0)
		{
			glm::vec2 overShoot = _feelers[feelerIndex] - closestPoint;
			steeringForceTemp = _walls->at(closestWall).getNormals() * glm::length(overShoot);
		}
	}

	return steeringForceTemp;
}
void SteeringBehaviors::tagNeighbors(float radius)
{
	for (auto currentAgent = _neighbors->begin(); currentAgent != _neighbors->end(); currentAgent++)
	{
		currentAgent->unTag();
		
		glm::vec2 to = currentAgent->getPos() - _agent->getPos();

		float range = radius + currentAgent->getBRadius();

		if (std::addressof(*currentAgent) != _agent && glm::length2(to) < range*range)
		{
			currentAgent->tag();
		}

	}
}

glm::vec2 SteeringBehaviors::separation()
{
	glm::vec2 steeringForce(0.0f);

	for (auto currentAgent = _neighbors->begin(); currentAgent != _neighbors->end(); currentAgent++)
	{
		if (std::addressof(*currentAgent) != _agent && currentAgent->isTagged())
		{
			glm::vec2 toAgent = _agent->getPos() - currentAgent->getPos();

			steeringForce += glm::normalize(toAgent) / glm::length(toAgent);
		}
	}

	return steeringForce;
}

glm::vec2 SteeringBehaviors::alignment()
{
	glm::vec2 averageDirection(0.0f);
	int neighborCount = 0;

	for (auto currentAgent = _neighbors->begin(); currentAgent != _neighbors->end(); currentAgent++)
	{
		if (std::addressof(*currentAgent) != _agent && currentAgent->isTagged())
		{
			averageDirection += currentAgent->getDirection();
			++neighborCount;
		}
	}

	if (neighborCount > 0)
	{
		averageDirection /= (float)neighborCount;
		averageDirection -= _agent->getDirection();
	}

	return averageDirection;
}

glm::vec2 SteeringBehaviors::cohesion()
{
	glm::vec2 centerOfMass(0.0f), steeringForce(0.0f);

	int neighborCount = 0;

	for (auto currentNeighbor = _neighbors->begin(); currentNeighbor != _neighbors->end(); currentNeighbor++)
	{
		if (std::addressof(*currentNeighbor) != _agent && currentNeighbor->isTagged())
		{
			centerOfMass += currentNeighbor->getPos();

			++neighborCount;
		}
	}

	if (neighborCount > 0)
	{
		centerOfMass /= (float)neighborCount;
		steeringForce = seek(centerOfMass);
	}

	return steeringForce;
}

void SteeringBehaviors::noOverlap()
{
	glm::vec2 toAgent;
	float distFromEachOther;
	float amoutOfOverLap;

	for (auto currentNeighbor = _neighbors->begin(); currentNeighbor != _neighbors->end(); currentNeighbor++)
	{
		if (std::addressof(*currentNeighbor) != _agent)
		{
			toAgent = _agent->getPos() - currentNeighbor->getPos();
			distFromEachOther = glm::length(toAgent);

			amoutOfOverLap = currentNeighbor->getBRadius() + _agent->getBRadius() - distFromEachOther;

			if (amoutOfOverLap >= 0)
			{
				_agent->setPos(_agent->getPos() + (toAgent / distFromEachOther) * amoutOfOverLap);
			}
		}
	}
}


bool SteeringBehaviors::accumulateForce(glm::vec2 &totalForce, glm::vec2 forceToAdd)
{
	//calculate how much steering force the vehicle has used so far
	float magnitudeSoFar = glm::length(totalForce);

	//calculate how much steering force remains to be used by this vehicle
	float magnitudeRemaining = _agent->getMaxForce() - magnitudeSoFar;

	//return false if there is no more force left to use
	if (magnitudeRemaining <= 0.0) return false;

	//calculate the magnitude of the force we want to add
	float magnitudeToAdd = glm::length(forceToAdd);

	//if the magnitude of the sum of forceToAdd and the running total
	//does not exceed the maximum force available to this vehicle, just
	//add together. Otherwise add as much of the forceToAdd vector is
	//possible without going over the max.
	if (magnitudeToAdd < magnitudeRemaining)
	{
		totalForce += forceToAdd;
	}

	else
	{
		//add it to the steering force
		totalForce += (glm::normalize(forceToAdd) * magnitudeRemaining);
	}

	return true;
}

glm::vec2 SteeringBehaviors::calculate()
{
	glm::vec2 force(0.0f);

	_steeringForce = glm::vec2(0.0f);

	/*if (_neighbors != NULL)
	{
		noOverlap();
	}*/

	if ((states[SteeringStates::separation] || states[SteeringStates::alignment] || states[SteeringStates::cohesion]) && _neighbors != NULL)
	{
		tagNeighbors(50.0f);
	}

	if (states[SteeringStates::wallAvoidance])
	{
		force = wallAvoidance() * weights[SteeringStates::wallAvoidance];

		//stop following target and move agent away from walls
		if (glm::length(force) > 0.0f && states[SteeringStates::arrive])
		{
			states[SteeringStates::arrive] = false;
		}

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::evade] && _agent->getTargetEntity() != NULL)
	{
		force = evade(_agent->getTargetEntity()) * weights[SteeringStates::evade];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::flee])
	{
		force = flee(_agent->getTarget()) * weights[SteeringStates::flee];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::separation] && _neighbors != NULL)
	{
		force = separation() * weights[SteeringStates::flee];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::alignment] && _neighbors != NULL)
	{
		force = alignment() * weights[SteeringStates::flee];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::cohesion] && _neighbors != NULL)
	{
		force = cohesion() * weights[SteeringStates::flee];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::seek])
	{
		force = seek(_agent->getTarget()) * weights[SteeringStates::seek];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::arrive])
	{
		force = arrive(_agent->getTarget(), Deceleration::slow) * weights[SteeringStates::arrive];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::wander])
	{
		force = wander() * weights[SteeringStates::wander];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::pursuit] && _agent->getTargetEntity() != NULL)
	{
		force = pursuit(_agent->getTargetEntity()) * weights[SteeringStates::pursuit];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}

	if (states[SteeringStates::offsetPursuit] && _agent->getTargetEntity() != NULL)
	{
		force = offsetPursuit(_agent->getTargetEntity(), _offset) * weights[SteeringStates::offsetPursuit];

		if (!accumulateForce(_steeringForce, force))
		{
			return _steeringForce;
		}
	}


	if (glm::length(_steeringForce) > _agent->getMaxForce())
	{
		_steeringForce = glm::normalize(_steeringForce);
		_steeringForce *= _agent->getMaxForce();
	}


	return _steeringForce;
}
