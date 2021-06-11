#ifndef MCLIB_ENTITY_ENTITY_MANAGER_H_
#define MCLIB_ENTITY_ENTITY_MANAGER_H_


#include <entity/Entity.h>
#include <entity/Player.h>
#include <protocol/packets/Packet.h>
#include <protocol/PacketHandler.h>
#include <util/ObserverSubject.h>


class EntityListener
{
public:
	virtual ~EntityListener() {}

	virtual void OnPlayerSpawn(PlayerEntityPtr entity, CUUID uuid) {}
	virtual void OnEntitySpawn(EntityPtr entity) {}
	virtual void OnObjectSpawn(EntityPtr entity) {}
	virtual void OnEntityDestroy(EntityPtr entity) {}
	virtual void OnEntityMove(EntityPtr entity, glm::dvec3 oldPos, glm::dvec3 newPos) {}
};


class EntityManager 
	: public PacketHandler
	, public ObserverSubject<EntityListener>
{
public:
	using EntityMap = std::unordered_map<EntityId, EntityPtr>;
	using iterator = EntityMap::iterator;
	using const_iterator = EntityMap::const_iterator;

public:
	MCLIB_API EntityManager(PacketDispatcher* dispatcher, Version protocolVersion);
	MCLIB_API ~EntityManager();

	EntityManager(const EntityManager& rhs) = delete;
	EntityManager& operator=(const EntityManager& rhs) = delete;
	EntityManager(EntityManager&& rhs) = delete;
	EntityManager& operator=(EntityManager&& rhs) = delete;

	std::shared_ptr<PlayerEntity> GetPlayerEntity() const
	{
		auto iter = m_Entities.find(m_ClientPlayerEntityId);
		if (iter == m_Entities.end()) 
			return nullptr;

		return std::dynamic_pointer_cast<PlayerEntity>(iter->second);
	}

	EntityPtr GetEntity(EntityId eid) const
	{
		auto iter = m_Entities.find(eid);
		if (iter == m_Entities.end()) 
			return nullptr;
		return iter->second;
	}

	iterator begin() { return m_Entities.begin(); }
	iterator end() { return m_Entities.end(); }

	const_iterator begin() const { return m_Entities.begin(); }
	const_iterator end() const { return m_Entities.end(); }

	void MCLIB_API HandlePacket(in::JoinGamePacket* packet);
	void MCLIB_API HandlePacket(in::PlayerPositionAndLookPacket* packet);
	void MCLIB_API HandlePacket(in::SpawnPlayerPacket* packet);
	void MCLIB_API HandlePacket(in::SpawnPaintingPacket* packet);
	void MCLIB_API HandlePacket(in::SpawnExperienceOrbPacket* packet);
	void MCLIB_API HandlePacket(in::SpawnGlobalEntityPacket* packet);
	void MCLIB_API HandlePacket(in::SpawnMobPacket* packet);
	void MCLIB_API HandlePacket(in::SpawnObjectPacket* packet);
	void MCLIB_API HandlePacket(in::EntityPacket* packet);
	void MCLIB_API HandlePacket(in::EntityRelativeMovePacket* packet);
	void MCLIB_API HandlePacket(in::EntityLookAndRelativeMovePacket* packet);
	void MCLIB_API HandlePacket(in::EntityTeleportPacket* packet);
	void MCLIB_API HandlePacket(in::EntityLookPacket* packet);
	void MCLIB_API HandlePacket(in::EntityHeadLookPacket* packet);
	void MCLIB_API HandlePacket(in::EntityVelocityPacket* packet);
	void MCLIB_API HandlePacket(in::EntityMetadataPacket* packet);
	void MCLIB_API HandlePacket(in::EntityPropertiesPacket* packet);
	void MCLIB_API HandlePacket(in::DestroyEntitiesPacket* packet);
	void MCLIB_API HandlePacket(in::AttachEntityPacket* packet);

private:
	std::unordered_map<EntityId, EntityPtr> m_Entities;
	EntityId m_ClientPlayerEntityId; // Entity Id for the client player
	Version m_ProtocolVersion;
};

#endif
