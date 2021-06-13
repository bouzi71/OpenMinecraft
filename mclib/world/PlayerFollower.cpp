#include "stdafx.h"

// General
#include "PlayerFollower.h"

// Additional
#include "util/Utility.h"

PlayerFollower::PlayerFollower(PacketDispatcher* dispatcher, CMinecraftClient* client)
	: m_Client(client),
	m_PlayerManager(*client->GetPlayerManager()),
	m_EntityManager(*client->GetEntityManager()),
	m_PlayerController(*client->GetPlayerController()),
	m_Target(L"")
{
	client->RegisterListener(this);
	m_PlayerManager.RegisterListener(this);
}

PlayerFollower::~PlayerFollower()
{
	m_Client->UnregisterListener(this);
	m_PlayerManager.UnregisterListener(this);
}

void PlayerFollower::UpdateRotation()
{
	if (!m_Following || !m_Following->GetEntity()) return;

	m_PlayerController.LookAt(m_Following->GetEntity()->GetPosition());
	/*static uint64 lastUpdate = GetTime();
	uint64 ticks = GetTime();
	float dt = (ticks - lastUpdate) / 1000.0f;
	lastUpdate = ticks;

	static float yaw = 0.0f;
	static float RotationsPerSecond = 0.6f;

	yaw += dt * 360.0f / (1.0f / RotationsPerSecond);*/

	// std::wcout << yaw << std::endl;

	//m_Pitch = (((float)std::sin(ticks * 0.5 * 3.14 / 1000) * 0.5f + 0.5f) * 360.0f) - 180.0f;
	//float yaw = (((float)std::sin(ticks * 0.5 * 3.14 / 1000))) * 360.0f;

	//std::wcout << yaw << std::endl;

	//m_PlayerController.SetYaw(yaw);
	//m_PlayerController.LookAt(m_Following->GetEntity()->GetPosition());
}

void PlayerFollower::OnTick()
{
	UpdateRotation();

	if (!m_Following || !m_Following->GetEntity())
	{
		m_PlayerController.SetTargetPosition(m_PlayerController.GetPosition());
		return;
	}

	auto entity = m_Following->GetEntity();
	EntityId vid = entity->GetVehicleId();
	glm::dvec3 targetPosition = entity->GetPosition();

	if (vid != -1)
		targetPosition = m_EntityManager.GetEntity(vid)->GetPosition();

	float yaw = (entity->GetYaw() / 256.0f) * 360.0f;
	float pitch = (entity->GetPitch() / 256.0f) * 360.0f;
	const float toRads = 3.14159f / 180.0f;

	if ((yaw += 90) >= 360.0) yaw -= 360;

	glm::dvec3 heading(
		std::cos(yaw * toRads) * std::cos(pitch * toRads),
		0.0,
		std::sin(yaw * toRads) * std::cos(pitch * toRads)
	);

	glm::dvec3 newPosition = targetPosition + glm::normalize(heading) * 0.4;

	if ((yaw -= 180.0f + 90.0f) < 0.0f) yaw += 360.0f;

	//m_PlayerController.SetYaw(yaw);
	m_PlayerController.SetMoveSpeed(4.3 * 1.3);
	m_PlayerController.SetTargetPosition(newPosition);
}

bool PlayerFollower::IsIgnored(const std::wstring& name)
{
	static const std::vector<std::wstring> IgnoreList = {
		L"Traced_", L"Judam"
	};

	return std::find(IgnoreList.begin(), IgnoreList.end(), name) != IgnoreList.end();
}

void PlayerFollower::FindClosestPlayer()
{
	double closest = std::numeric_limits<double>::max();


	m_Following = nullptr;

	if (!m_Target.empty())
	{
		for (auto& kv : m_PlayerManager)
		{
			auto player = kv.second;
			auto entity = player->GetEntity();

			if (entity && player->GetName() == m_Target)
				m_Following = player;
		}
	}

	if (!m_Following)
	{
		for (auto& kv : m_PlayerManager)
		{
			PlayerPtr player = kv.second;

			if (IsIgnored(player->GetName())) continue;

			auto entity = player->GetEntity();

			if (!entity) continue;

			EntityId peid = m_EntityManager.GetPlayerEntity()->GetEntityId();
			if (entity->GetEntityId() == peid) continue;

			glm::dvec3 pos = entity->GetPosition();
			EntityId vid = entity->GetVehicleId();
			if (vid != -1)
			{
				EntityPtr vehicle = m_EntityManager.GetEntity(vid);
				if (vehicle)
					pos = vehicle->GetPosition();
			}

			double dist = glm::distance(pos, m_PlayerController.GetPosition());

			if (dist < closest)
			{
				closest = dist;
				m_Following = player;
			}
		}
	}

	static uint64 lastOutput = 0;

	if (GetTime() - lastOutput >= 3000)
	{
		lastOutput = GetTime();
		if (m_Following)
		{
			auto entity = m_Following->GetEntity();
			if (entity)
			{
				glm::dvec3 pos = entity->GetPosition();
				EntityId vid = entity->GetVehicleId();
				if (vid != -1)
				{
					EntityPtr vehicle = m_EntityManager.GetEntity(vid);
					if (vehicle)
						pos = vehicle->GetPosition();
				}
				double dist = glm::distance(m_PlayerController.GetPosition(), pos);
				std::wstring followName = m_Following->GetName();

				std::string followMesg = "Tracking " + std::string(followName.begin(), followName.end()) + " dist: " + std::to_string(dist) + "\n";
				Log::Print(followMesg.c_str());
			}
		}
	}

	if (!m_Following)
	{
		Log::Print("Not tracking anyone\n");
	}
	else
		m_PlayerController.SetTargetPosition(m_Following->GetEntity()->GetPosition());
}

void PlayerFollower::OnPlayerJoin(PlayerPtr player)
{
	FindClosestPlayer();
	UpdateRotation();
}

void PlayerFollower::OnPlayerLeave(PlayerPtr player)
{
	FindClosestPlayer();
	UpdateRotation();
}

void PlayerFollower::OnPlayerSpawn(PlayerPtr player)
{
	FindClosestPlayer();
	UpdateRotation();
}

void PlayerFollower::OnPlayerDestroy(PlayerPtr player, EntityId eid)
{
	FindClosestPlayer();
	UpdateRotation();
}

void PlayerFollower::OnPlayerMove(PlayerPtr player, glm::dvec3 oldPos, glm::dvec3 newPos)
{
	FindClosestPlayer();
	UpdateRotation();
}