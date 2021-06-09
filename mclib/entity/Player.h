#ifndef MCLIB_ENTITY_ENTITY_PLAYER_H_
#define MCLIB_ENTITY_ENTITY_PLAYER_H_

#include <entity/LivingEntity.h>

class PlayerEntity 
	: public LivingEntity
{
public:
	PlayerEntity(EntityId id, Version protocolVersion) : LivingEntity(id, protocolVersion) {}
};

typedef std::weak_ptr<PlayerEntity> PlayerEntityPtr;

#endif
