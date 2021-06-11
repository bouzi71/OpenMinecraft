#ifndef MCLIB_ENTITY_ENTITY_FACTORY_H_
#define MCLIB_ENTITY_ENTITY_FACTORY_H_

#include <entity/Entity.h>

class EntityFactory
{
public:
	static Entity* CreateEntity(EntityId id, uint8 type);
};

#endif
