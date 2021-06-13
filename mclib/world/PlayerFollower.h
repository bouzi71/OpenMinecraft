#pragma once

#include "core/Client.h"
#include "core/PlayerManager.h"

#include "world/PlayerController.h"

class PlayerFollower
	: public PlayerListener
	, public ClientListener
{
public:
	MCLIB_API PlayerFollower(PacketDispatcher* dispatcher, CMinecraftClient* client);

	MCLIB_API ~PlayerFollower();

	MCLIB_API void UpdateRotation();

	MCLIB_API void OnTick() override;
	MCLIB_API bool IsIgnored(const std::wstring& name);

	MCLIB_API void FindClosestPlayer();

	MCLIB_API void OnPlayerJoin(PlayerPtr player) override;
	MCLIB_API void OnPlayerLeave(PlayerPtr player) override;
	MCLIB_API void OnPlayerSpawn(PlayerPtr player) override;
	MCLIB_API void OnPlayerDestroy(PlayerPtr player, EntityId eid) override;
	MCLIB_API void OnPlayerMove(PlayerPtr player, glm::dvec3 oldPos, glm::dvec3 newPos) override;

private:
	CMinecraftClient* m_Client;
	PlayerManager& m_PlayerManager;
	EntityManager& m_EntityManager;
	PlayerPtr m_Following;
	PlayerController& m_PlayerController;
	std::wstring m_Target;
	uint64 m_LastUpdate;
};