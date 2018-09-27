#pragma once

#include <glm/glm.hpp>
#include <algorithm>


class BaseEntity
{
public:
	enum { default_entity_type = -1 };

private:
	int _id;
	int _entityType;
	bool _bTag;
	int nextValidID() { static int nextID = 0; return nextID++; }

protected:

	glm::vec2 _position;
	glm::vec2 _scale;
	float _boundingRadius;

	BaseEntity() :_id(nextValidID()), _boundingRadius(0.0), _position(glm::vec2()), _scale(glm::vec2(1.0f)), _entityType(default_entity_type), _bTag(false) {}

	BaseEntity(int entityType) : _id(nextValidID()), _boundingRadius(0.0), _position(glm::vec2()), _scale(glm::vec2(1.0f)), _entityType(entityType), _bTag(false) {}

	BaseEntity(int entityType, glm::vec2& pos, float r) : _id(nextValidID()), _boundingRadius(r), _position(pos), _scale(glm::vec2(1.0f)), _entityType(entityType), _bTag(false) {}

public:
	virtual ~BaseEntity() {}

	virtual void init() {};

	virtual void update(float deltaTime) {};

	virtual void draw() {};

	glm::vec2 getPos()const { return _position; }
	void setPos(glm::vec2 newPos) { _position = newPos; }

	float getBRadius()const { return _boundingRadius; }
	void setBRadius(float r) { _boundingRadius = r; }

	int getID()const { return _id; }

	bool isTagged()const { return _bTag; }
	void tag() { _bTag = true; }
	void unTag() { _bTag = false; }

	glm::vec2 getScale()const { return _scale; }
	void setScale(glm::vec2 newScale)
	{
		_boundingRadius *= std::max(newScale.x, newScale.y) / std::max(_scale.x, _scale.y);
		_scale = newScale;
	}
	void setScale(float newVal)
	{
		_boundingRadius *= (newVal / std::max(_scale.x, _scale.y));
		_scale = glm::vec2(newVal, newVal);
	}

	int getEntityType()const { return _entityType; }
	void setEntityType(int newType) { _entityType = newType; }

};

