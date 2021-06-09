#include "stdafx.h"

#include <core/PlayerManager.h>

#include <protocol/PacketDispatcher.h>


PlayerManager::PlayerManager(PacketDispatcher* dispatcher, EntityManager* entityManager)
	: PacketHandler(dispatcher)
	, m_EntityManager(entityManager)
{
	dispatcher->RegisterHandler(State::Login, login::LoginSuccess, this);

	dispatcher->RegisterHandler(State::Play, play::PlayerListItem, this);
	dispatcher->RegisterHandler(State::Play, play::PlayerPositionAndLook, this);

	m_EntityManager->RegisterListener(this);
}

PlayerManager::~PlayerManager()
{
	m_EntityManager->UnregisterListener(this);
	GetDispatcher()->UnregisterHandler(this);
}

PlayerManager::iterator PlayerManager::begin()
{
	return m_Players.begin();
}

PlayerManager::iterator PlayerManager::end()
{
	return m_Players.end();
}



PlayerPtr PlayerManager::GetPlayerByUUID(CUUID uuid) const
{
	auto iter = m_Players.find(uuid);
	if (iter != m_Players.end())
		return iter->second;

	return nullptr;
}

PlayerPtr PlayerManager::GetPlayerByEntityId(EntityId eid) const
{
	auto iter = std::find_if(m_Players.begin(), m_Players.end(), [eid](std::pair<CUUID, PlayerPtr> kv) {
		PlayerPtr player = kv.second;
		auto ptr = player->GetEntity();
		if (!ptr) return false;
		return ptr->GetEntityId() == eid;
	});

	if (iter != m_Players.end())
		return iter->second;

	return nullptr;
}

PlayerPtr PlayerManager::GetPlayerByName(const std::wstring& name) const
{
	auto iter = std::find_if(m_Players.begin(), m_Players.end(), [name](std::pair<CUUID, PlayerPtr> kv) {
		PlayerPtr player = kv.second;
		return player->GetName().compare(name) == 0;
	});

	if (iter != m_Players.end())
		return iter->second;

	return nullptr;
}



//
// EntityListener
//
void PlayerManager::OnPlayerSpawn(PlayerEntityPtr entity, CUUID uuid)
{
	auto iter = m_Players.find(uuid);
	if (iter == m_Players.end())
		m_Players[uuid] = std::make_shared<Player>(uuid, L"");

	m_Players[uuid]->SetEntity(entity);

	NotifyListeners(&PlayerListener::OnPlayerSpawn, m_Players[uuid]);
}

void PlayerManager::OnEntityDestroy(EntityPtr entity)
{
	EntityId eid = entity->GetEntityId();

	auto player = GetPlayerByEntityId(eid);

	if (player)
	{
		player->SetEntity(PlayerEntityPtr());
		NotifyListeners(&PlayerListener::OnPlayerDestroy, player, eid);
	}
}

void PlayerManager::OnEntityMove(EntityPtr entity, Vector3d oldPos, Vector3d newPos)
{
	EntityId eid = entity->GetEntityId();
	PlayerPtr player = GetPlayerByEntityId(eid);

	if (!player) return;

	NotifyListeners(&PlayerListener::OnPlayerMove, player, oldPos, newPos);
}



//
// PacketHandler
//
void PlayerManager::HandlePacket(login::in::LoginSuccessPacket* packet)
{
	m_ClientUUID = CUUID::FromString(packet->GetUUID());

	printf("PlayerManager: LoginSuccessPacket received. UUID: '%s'. Username: '%s'.\r\n", utf16to8(packet->GetUUID()).c_str(), utf16to8(packet->GetUsername()).c_str());
}

void PlayerManager::HandlePacket(in::PlayerPositionAndLookPacket* packet)
{
	auto player = m_EntityManager->GetPlayerEntity();

	auto iter = m_Players.find(m_ClientUUID);
	if (iter == m_Players.end())
	{
		m_Players[m_ClientUUID] = std::make_shared<Player>(m_ClientUUID, L"");
	}
	m_Players[m_ClientUUID]->SetEntity(player);

	NotifyListeners(&PlayerListener::OnClientSpawn, m_Players[m_ClientUUID]);
}

void PlayerManager::HandlePacket(in::PlayerListItemPacket* packet)
{
	using namespace in;

	auto action = packet->GetAction();
	const auto& actionDataList = packet->GetActionData();

	for (const auto& actionData : actionDataList)
	{
		CUUID uuid = actionData->uuid;

		if (action == PlayerListItemPacket::Action::AddPlayer)
		{
			auto iter = m_Players.find(uuid);
			if (iter != m_Players.end())
			{
				bool newPlayer = iter->second->m_Name.empty();
				if (newPlayer)
				{
					iter->second->m_Name = actionData->name;
					NotifyListeners(&PlayerListener::OnPlayerJoin, m_Players[uuid]);
				}
				continue;
			}

			PlayerPtr player = std::make_shared<Player>(uuid, actionData->name);
			m_Players[uuid] = player;

			NotifyListeners(&PlayerListener::OnPlayerJoin, m_Players[uuid]);
		}
		else if (action == PlayerListItemPacket::Action::RemovePlayer)
		{
			if (m_Players.find(uuid) == m_Players.end()) 
				continue;

			NotifyListeners(&PlayerListener::OnPlayerLeave, m_Players[uuid]);

			m_Players.erase(uuid);
		}
	}
}
