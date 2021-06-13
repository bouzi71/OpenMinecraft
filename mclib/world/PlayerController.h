#pragma once

#include "block/Block.h"
#include "core/PlayerManager.h"
#include "world/World.h"

class PlayerController
	: public PlayerListener
{
private:
	PlayerManager& m_PlayerManager;
	Connection* m_Connection;
	World& m_World;
	glm::dvec3 m_Position;
	float m_Yaw;
	float m_Pitch;
	BoundingBox m_BoundingBox;
	EntityId m_EntityId;
	uint64 m_LastUpdate;
	glm::dvec3 m_TargetPos;
	bool m_Sprinting;
	bool m_LoadedIn;
	bool m_HandleFall;

	double m_MoveSpeed;

	std::queue<glm::dvec3> m_DigQueue;

	// todo: gravity
	const float FallSpeed = 8.3f * (50.0f / 1000.0f);

	std::vector<std::pair<const CMinecraftBlock*, glm::ivec3>> GetNearbyBlocks(const int32 radius);

public:
	MCLIB_API PlayerController(Connection* connection, World& world, PlayerManager& playerManager);
	MCLIB_API ~PlayerController();

	void MCLIB_API OnClientSpawn(PlayerPtr player);
	bool MCLIB_API ClearPath(glm::dvec3 target);

	void MCLIB_API Dig(glm::dvec3 target);
	void MCLIB_API Attack(EntityId id);
	void MCLIB_API Move(glm::dvec3 delta);

	bool MCLIB_API HandleJump();
	bool MCLIB_API HandleFall();
	void MCLIB_API UpdateDigging();
	void MCLIB_API UpdatePosition();
	void MCLIB_API Update();

	bool MCLIB_API InLoadedChunk() const;
	glm::dvec3 MCLIB_API GetPosition() const;
	glm::dvec3 MCLIB_API GetHeading() const;
	float MCLIB_API GetYaw() const;
	float MCLIB_API GetPitch() const;
	BoundingBox MCLIB_API GetBoundingBox() const;

	void MCLIB_API SetYaw(float yaw);
	void MCLIB_API SetPitch(float pitch);
	void MCLIB_API LookAt(glm::dvec3 target);
	void MCLIB_API SetMoveSpeed(double speed);
	void MCLIB_API SetTargetPosition(glm::dvec3 target);
	void MCLIB_API SetHandleFall(bool handle);
};