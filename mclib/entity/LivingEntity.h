#ifndef MCLIB_ENTITY_LIVING_ENTITY_H_
#define MCLIB_ENTITY_LIVING_ENTITY_H_

#include <entity/Entity.h>

class LivingEntity 
	: public Entity
{
public:
	LivingEntity(EntityId id, Version protocolVersion) : Entity(id, protocolVersion) {}

	float GetHealth() const { return m_Health; }

private:
	float m_Health;
};

#endif
