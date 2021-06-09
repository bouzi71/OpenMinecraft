#include "stdafx.h"

#include <entity/EntityManager.h>

#include <entity/Painting.h>
#include <entity/XPOrb.h>
#include <protocol/PacketDispatcher.h>


#include <algorithm>
#include <iostream>

// todo: Use factories to create the entities

#define TAU 3.14159f * 2.0f
#define DEG_TO_RAD 3.14159f / 180.0f

EntityType GetEntityTypeFromObjectId(s32 oid)
{
	static const std::unordered_map<s32, EntityType> mapping = {
		{ 1, EntityType::Boat },
		{ 2, EntityType::Item },
		{ 3, EntityType::AreaEffectCloud },
		{ 10, EntityType::MinecartRideable },
		{ 50, EntityType::PrimedTnt },
		{ 51, EntityType::EnderCrystal },
		{ 60, EntityType::Arrow },
		{ 61, EntityType::Snowball },
		{ 62, EntityType::ThrownEgg },
		{ 63, EntityType::Fireball },
		{ 64, EntityType::SmallFireball },
		{ 65, EntityType::ThrownEnderpearl },
		{ 66, EntityType::WitherSkull },
		{ 67, EntityType::ShulkerBullet },
		{ 68, EntityType::LlamaSpit },
		{ 70, EntityType::FallingObject },
		{ 71, EntityType::ItemFrame },
		{ 72, EntityType::EyeOfEnderSignal },
		{ 73, EntityType::ThrownPotion },
		{ 75, EntityType::ThrownExpBottle },
		{ 76, EntityType::FireworksRocketEntity },
		{ 77, EntityType::LeashKnot },
		{ 78, EntityType::ArmorStand },
		{ 79, EntityType::EvocationFangs },
		{ 90, EntityType::FishingHook },
		{ 91, EntityType::SpectralArrow },
		{ 93, EntityType::DragonFireball }
	};

	return mapping.at(oid);
}

EntityManager::EntityManager(PacketDispatcher* dispatcher, Version protocolVersion)
	: PacketHandler(dispatcher)
	, m_ProtocolVersion(protocolVersion)
{
	GetDispatcher()->RegisterHandler(State::Play, play::JoinGame, this);
	GetDispatcher()->RegisterHandler(State::Play, play::PlayerPositionAndLook, this);
	GetDispatcher()->RegisterHandler(State::Play, play::SpawnPlayer, this);
	GetDispatcher()->RegisterHandler(State::Play, play::SpawnPainting, this);
	GetDispatcher()->RegisterHandler(State::Play, play::SpawnExperienceOrb, this);
	GetDispatcher()->RegisterHandler(State::Play, play::SpawnGlobalEntity, this);
	GetDispatcher()->RegisterHandler(State::Play, play::SpawnMob, this);
	GetDispatcher()->RegisterHandler(State::Play, play::SpawnObject, this);
	GetDispatcher()->RegisterHandler(State::Play, play::Entity, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityRelativeMove, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityLookAndRelativeMove, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityTeleport, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityLook, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityHeadLook, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityMetadata, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityVelocity, this);
	GetDispatcher()->RegisterHandler(State::Play, play::EntityProperties, this);
	GetDispatcher()->RegisterHandler(State::Play, play::DestroyEntities, this);
	GetDispatcher()->RegisterHandler(State::Play, play::AttachEntity, this);
}

EntityManager::~EntityManager()
{
	GetDispatcher()->UnregisterHandler(this);
}

void EntityManager::HandlePacket(in::AttachEntityPacket* packet)
{
	EntityId eid = packet->GetEntityId();
	EntityId vid = packet->GetVehicleId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;
	if (entity)
		entity->SetVehicleId(vid);
}

void EntityManager::HandlePacket(in::JoinGamePacket* packet)
{
	EntityId id = packet->GetEntityId();
	m_ClientPlayerEntityId = id;

	std::shared_ptr<PlayerEntity> entity = std::make_shared<PlayerEntity>(id, m_ProtocolVersion);
	m_Entities[id] = entity;
}

void EntityManager::HandlePacket(in::PlayerPositionAndLookPacket* packet)
{
	auto iter = m_Entities.find(m_ClientPlayerEntityId);
	EntityPtr entity;

	if (iter == m_Entities.end())
	{
		entity = std::make_shared<PlayerEntity>(m_ClientPlayerEntityId, m_ProtocolVersion);
	}
	else
	{
		entity = iter->second;
	}

	if (entity)
	{
		entity->SetPosition(packet->GetPosition());
		entity->SetYaw(packet->GetYaw() * DEG_TO_RAD);
		entity->SetPitch(packet->GetPitch() * DEG_TO_RAD);
	}
}

void EntityManager::HandlePacket(in::SpawnPlayerPacket* packet)
{
	EntityId id = packet->GetEntityId();

	std::shared_ptr<PlayerEntity> entity = std::make_shared<PlayerEntity>(id, m_ProtocolVersion);

	m_Entities[id] = entity;

	entity->SetType(EntityType::Player);
	entity->SetPosition(packet->GetPosition());
	entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
	entity->SetPitch(packet->GetPitch() / 256.0f * TAU);
	entity->SetMetadata(packet->GetMetadata());

	CUUID uuid = packet->GetUUID();

	NotifyListeners(&EntityListener::OnPlayerSpawn, PlayerEntityPtr(entity), uuid);
	NotifyListeners(&EntityListener::OnEntitySpawn, entity);
}

void EntityManager::HandlePacket(in::SpawnObjectPacket* packet)
{
	EntityId eid = packet->GetEntityId();
	EntityPtr entity = std::make_shared<Entity>(eid, m_ProtocolVersion);

	m_Entities[eid] = entity;
	entity->SetPosition(ToVector3d(packet->GetPosition()));
	entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
	entity->SetPitch(packet->GetPitch() / 256.0f * TAU);

	Vector3d velocity = ToVector3d(packet->GetVelocity()) / 8000.0;
	entity->SetVelocity(velocity);

	EntityType type = GetEntityTypeFromObjectId(packet->GetType());
	entity->SetType(type);

	NotifyListeners(&EntityListener::OnObjectSpawn, entity);
}

void EntityManager::HandlePacket(in::SpawnPaintingPacket* packet)
{
	EntityId eid = packet->GetEntityId();
	auto entity = std::make_shared<PaintingEntity>(eid, m_ProtocolVersion);

	m_Entities[eid] = entity;

	entity->SetPosition(ToVector3d(packet->GetPosition()));
	entity->SetType(EntityType::Painting);
	entity->SetTitle(packet->GetTitle());
	entity->SetDirection((PaintingEntity::Direction)packet->GetDirection());
}

void EntityManager::HandlePacket(in::SpawnExperienceOrbPacket* packet)
{
	EntityId eid = packet->GetEntityId();
	EntityPtr entity = std::make_shared<XPOrb>(eid, packet->GetCount(), m_ProtocolVersion);

	m_Entities[eid] = entity;

	entity->SetPosition(packet->GetPosition());
	entity->SetType(EntityType::XPOrb);
}

void EntityManager::HandlePacket(in::SpawnGlobalEntityPacket* packet)
{
	EntityId eid = packet->GetEntityId();
	EntityPtr entity = std::make_shared<Entity>(eid, m_ProtocolVersion);

	m_Entities[eid] = entity;

	entity->SetPosition(packet->GetPosition());
	entity->SetType(EntityType::Lightning);
}

void EntityManager::HandlePacket(in::SpawnMobPacket* packet)
{
	EntityId eid = packet->GetEntityId();
	EntityPtr entity = std::make_shared<Entity>(eid, m_ProtocolVersion);

	m_Entities[eid] = entity;

	entity->SetType((EntityType)packet->GetType());
	entity->SetPosition(packet->GetPosition());
	entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
	entity->SetPitch(packet->GetPitch() / 256.0f * TAU);
	entity->SetHeadPitch(packet->GetHeadPitch() / 256.0f * TAU);
	entity->SetMetadata(packet->GetMetadata());

	Vector3d velocity = ToVector3d(packet->GetVelocity()) / 8000.0;
	entity->SetVelocity(velocity);

	NotifyListeners(&EntityListener::OnEntitySpawn, entity);
}

void EntityManager::HandlePacket(in::DestroyEntitiesPacket* packet)
{
	std::vector<EntityId> eids = packet->GetEntityIds();

	for (EntityId eid : eids)
	{
		auto iter = m_Entities.find(eid);
		if (iter == m_Entities.end()) continue;

		auto entity = iter->second;
		if (entity)
			NotifyListeners(&EntityListener::OnEntityDestroy, iter->second);

		m_Entities.erase(eid);
	}
}

void EntityManager::HandlePacket(in::EntityPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);

	if (iter == m_Entities.end())
	{
		EntityPtr entity = std::make_shared<Entity>(eid, m_ProtocolVersion);

		m_Entities[eid] = entity;
	}
}

void EntityManager::HandlePacket(in::EntityVelocityPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;

	if (entity)
	{
		Vector3d velocity = ToVector3d(packet->GetVelocity()) / 8000.0;

		entity->SetVelocity(velocity);
	}
}

void EntityManager::HandlePacket(in::EntityRelativeMovePacket* packet)
{
	EntityId eid = packet->GetEntityId();

	Vector3d delta = ToVector3d(packet->GetDelta()) / (32.0 * 128.0);

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;

	if (entity)
	{
		Vector3d oldPos = entity->GetPosition();
		Vector3d newPos = entity->GetPosition() + delta;

		entity->SetPosition(newPos);

		NotifyListeners(&EntityListener::OnEntityMove, entity, oldPos, newPos);
	}
}

void EntityManager::HandlePacket(in::EntityLookAndRelativeMovePacket* packet)
{
	EntityId eid = packet->GetEntityId();

	Vector3d delta = ToVector3d(packet->GetDelta()) / (32.0 * 128.0);

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;

	if (entity)
	{
		Vector3d oldPos = entity->GetPosition();
		Vector3d newPos = entity->GetPosition() + delta;

		entity->SetPosition(newPos);
		entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
		entity->SetPitch(packet->GetPitch() / 256.0f * TAU);

		NotifyListeners(&EntityListener::OnEntityMove, entity, oldPos, newPos);
	}
}

void EntityManager::HandlePacket(in::EntityTeleportPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;

	if (entity)
	{
		Vector3d oldPos = entity->GetPosition();
		Vector3d newPos = packet->GetPosition();

		entity->SetPosition(newPos);
		entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
		entity->SetPitch(packet->GetPitch() / 256.0f * TAU);

		NotifyListeners(&EntityListener::OnEntityMove, entity, oldPos, newPos);
	}
}

void EntityManager::HandlePacket(in::EntityLookPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;
	if (entity)
	{
		entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
		entity->SetPitch(packet->GetPitch() / 256.0f * TAU);
	}
}

void EntityManager::HandlePacket(in::EntityHeadLookPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;
	if (entity)
		entity->SetYaw(packet->GetYaw() / 256.0f * TAU);
}

void EntityManager::HandlePacket(in::EntityMetadataPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;
	if (entity)
	{
		entity->SetMetadata(packet->GetMetadata());
	}
}

void EntityManager::HandlePacket(in::EntityPropertiesPacket* packet)
{
	auto eid = packet->GetEntityId();

	auto iter = m_Entities.find(eid);
	if (iter == m_Entities.end()) return;

	auto entity = iter->second;
	if (entity)
	{
		const auto& props = packet->GetProperties();

		for (const auto& entry : props)
		{
			const auto& key = entry.first;
			const auto& attrib = entry.second;

			entity->SetAttribute(key, attrib);
		}
	}
}
