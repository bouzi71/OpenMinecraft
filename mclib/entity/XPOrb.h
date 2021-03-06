#ifndef MCLIB_ENTITY_XPORB_H_
#define MCLIB_ENTITY_XPORB_H_

#include <entity/Entity.h>

class XPOrb 
	: public Entity
{
private:
	uint16 m_Count;

public:
	XPOrb(EntityId eid, Version protocolVersion) : Entity(eid, protocolVersion), m_Count(0) { SetType(EntityType::XPOrb); }
	XPOrb(EntityId eid, uint16 count, Version protocolVersion) : Entity(eid, protocolVersion), m_Count(count) { SetType(EntityType::XPOrb); }

	inline uint16 GetCount() const noexcept { return m_Count; }
	void SetCount(uint16 count) { m_Count = count; }
};

#endif
