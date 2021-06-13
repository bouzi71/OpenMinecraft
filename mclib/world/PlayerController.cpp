#include "stdafx.h"

// General
#include "PlayerController.h"

// Additional
#include "util/Utility.h"

PlayerController::PlayerController(Connection* connection, World& world, PlayerManager& playerManager)
	: m_PlayerManager(playerManager),
	m_Connection(connection),
	m_World(world),
	m_Position(0, 0, 0),
	m_BoundingBox(glm::dvec3(-0.3, 0, -0.3), glm::dvec3(0.3, 1.8, 0.3)),
	m_EntityId(-1),
	m_LastUpdate(GetTime()),
	m_Sprinting(false),
	m_LoadedIn(false),
	m_MoveSpeed(4.3),
	m_HandleFall(true)
{
	m_PlayerManager.RegisterListener(this);

	//console.SetImpl(new LoggerConsole("PlayerController.log"));
	//console.SetImpl(new StandardConsole());
}

PlayerController::~PlayerController()
{
	m_PlayerManager.UnregisterListener(this);
}

BoundingBox PlayerController::GetBoundingBox() const
{
	return m_BoundingBox + m_Position;
}

bool PlayerController::ClearPath(glm::dvec3 target)
{
	return true;
	glm::dvec3 position = m_Position;

	double dist = glm::distance(target, position);

	glm::dvec3 toTarget = target - position;
	glm::dvec3 n = glm::normalize(toTarget);

	glm::dvec3 side = glm::cross(n, glm::dvec3(0, 1, 0));

	const double CheckWidth = 0.3;

	auto check = [&](glm::dvec3 start, glm::dvec3 delta) {
		glm::dvec3 checkAbove = start + delta + glm::dvec3(0, 1, 0);
		glm::dvec3 checkBelow = start + delta + glm::dvec3(0, 0, 0);
		const CMinecraftBlock* aboveBlock = m_World.GetBlock(checkAbove);
		const CMinecraftBlock* belowBlock = m_World.GetBlock(checkBelow);

		if (aboveBlock && aboveBlock->IsSolid())
			return false;

		if (belowBlock && belowBlock->IsSolid())
		{
			const CMinecraftBlock* twoAboveBlock = m_World.GetBlock(checkAbove + glm::dvec3(0, 1, 0));
			// Bad path if there isn't a two high gap in it
			if (twoAboveBlock && twoAboveBlock->IsSolid())
				return false;

			// Jump up 1 block to keep searching
			position += glm::dvec3(0, 1, 0);
		}
		return true;
	};

	for (int32 i = 0; i < (int)std::ceil(dist); ++i)
	{
		glm::dvec3 delta(i * n.x, 0, i * n.z);

		// Check right side
		if (!check(position + side * CheckWidth, delta)) return false;
		if (!check(position - side * CheckWidth, delta)) return false;

		glm::dvec3 checkFloor = position + delta + glm::dvec3(0, -1, 0);
		const CMinecraftBlock* floorBlock = m_World.GetBlock(checkFloor);
		if (floorBlock && !floorBlock->IsSolid())
		{
			const CMinecraftBlock* belowFloorBlock = m_World.GetBlock(checkFloor - glm::dvec3(0, 1, 0));

			// Fail if there is a two block drop
			if (belowFloorBlock && !belowFloorBlock->IsSolid())
				return false;

			position.y--;
		}
	}

	return true;
}

void PlayerController::SetMoveSpeed(double speed) { m_MoveSpeed = speed; }

void PlayerController::OnClientSpawn(PlayerPtr player)
{
	m_Yaw = player->GetEntity()->GetYaw();
	m_Pitch = player->GetEntity()->GetPitch();
	m_Position = player->GetEntity()->GetPosition();
	m_LoadedIn = true;
	m_TargetPos = m_Position;
	auto entity = player->GetEntity();
	if (entity)
	{
		EntityId eid = entity->GetEntityId();

		m_EntityId = eid;
	}
}

void PlayerController::Dig(glm::dvec3 target)
{
	glm::dvec3 toTarget = target - m_Position;

	if (glm::length(toTarget) > 6)
		return;

	m_DigQueue.push(target);
}

void PlayerController::Attack(EntityId id)
{
	static uint64 timer = 0;
	static const uint64 cooldown = 500;

	if (GetTime() - timer < cooldown)
		return;

	using namespace out;

	UseEntityPacket useEntity(id, UseEntityPacket::Action::Attack);
	m_Connection->SendPacket(&useEntity);

	timer = GetTime();
}

void PlayerController::UpdateDigging()
{
	if (m_DigQueue.empty())
		return;

	//glm::dvec3 target = m_DigQueue.front();

}

std::vector<std::pair<const CMinecraftBlock*, glm::ivec3>> PlayerController::GetNearbyBlocks(const int32 radius)
{
	using BlockPos = std::pair<const CMinecraftBlock*, glm::ivec3>;

	std::vector<BlockPos> nearbyBlocks;

	for (int32 x = -radius; x < radius; ++x)
	{
		for (int32 y = -radius; y < radius; ++y)
		{
			for (int32 z = -radius; z < radius; ++z)
			{
				glm::dvec3 checkPos = m_Position + glm::dvec3(x, y, z);

				auto state = m_World.GetBlock(checkPos);

				if (state && state->IsSolid())
					nearbyBlocks.push_back(std::make_pair<>(state, checkPos));
			}
		}
	}

	std::sort(nearbyBlocks.begin(), nearbyBlocks.end(), [](const BlockPos& a, const BlockPos& b) {
		return a.second < b.second;
	});

	nearbyBlocks.erase(std::unique(nearbyBlocks.begin(), nearbyBlocks.end()), nearbyBlocks.end());

	return nearbyBlocks;
}

bool PlayerController::HandleJump()
{
	BoundingBox playerBounds = m_BoundingBox + m_Position;

	for (const auto& state : GetNearbyBlocks(2))
	{
		auto checkBlock = state.first;
		auto pos = state.second;

		auto result = checkBlock->CollidesWith(pos, playerBounds);
		if (result.first)
		{
			m_Position.y = result.second.getMax().y;
			return true;
		}
	}

	return false;
}

bool PlayerController::HandleFall()
{
	double bestDist = FallSpeed;
	CMinecraftBlock* bestBlock = nullptr;

	if (!InLoadedChunk())
		return false;

	BoundingBox playerBounds = m_BoundingBox + (m_Position - glm::dvec3(0, FallSpeed, 0));

	for (const auto& state : GetNearbyBlocks(2))
	{
		auto checkBlock = state.first;

		auto boundingBoxes = checkBlock->GetBoundingBoxes();
		for (auto blockBounds : boundingBoxes)
		{
			BoundingBox checkBounds = blockBounds + state.second;

			if (checkBounds.Intersects(playerBounds))
			{
				double penetration = checkBounds.getMax().y - playerBounds.getMin().y;
				double dist = FallSpeed - penetration;

				if (dist < bestDist)
					bestDist = dist;
			}
		}
	}

	m_Position -= glm::dvec3(0.0, bestDist, 0.0);

	return bestDist == FallSpeed;
}

void PlayerController::SetTargetPosition(glm::dvec3 target)
{
	m_TargetPos = target;
}

void PlayerController::SetHandleFall(bool handle)
{
	m_HandleFall = handle;
}

void PlayerController::UpdatePosition()
{
	//static const double WalkingSpeed = 4.3; // m/s
	//static const double WalkingSpeed = 8.6; // m/s
	static const double TicksPerSecond = 20;

	uint64 dt = GetTime() - m_LastUpdate;

	if (dt < 1000 / TicksPerSecond)
		return;

	m_LastUpdate = GetTime();

	static int counter = 0;

	if (++counter == 30)
	{
		Log::Info("PlayerController: Current pos: %f, %f, %f.", GetPosition().x, GetPosition().y, GetPosition().z);
		counter = 0;
	}

	glm::dvec3 target = m_TargetPos;
	glm::dvec3 toTarget = target - GetPosition();
	toTarget.y = 0;
	double dist = glm::length(toTarget);

	if (!ClearPath(target))
		return;

	glm::dvec3 n = glm::normalize(toTarget);
	double change = dt / 1000.0;

	n *= m_MoveSpeed * change;

	if (glm::length(n) > dist)
		n = glm::normalize(n) * dist;

	Move(n);
}

void PlayerController::Update()
{
	if (!m_LoadedIn) return;

	UpdatePosition();

	bool onGround = true;

	if (HandleJump())
	{
		Log::Info("PlayerController: Jumping.");
	}
	else
	{
		if (!m_HandleFall)
		{
			const float FullCircle = 2.0f * 3.14159f;
			const float CheckWidth = 0.3f;
			onGround = false;
			for (float angle = 0.0f; angle < FullCircle; angle += FullCircle / 8)
			{
				glm::dvec3 checkPos = m_Position + Vector3RotateAboutY(glm::dvec3(0, 0, CheckWidth), angle) - glm::dvec3(0, 1, 0);

				const CMinecraftBlock* checkBlock = m_World.GetBlock(checkPos);
				if (checkBlock && checkBlock->IsSolid())
				{
					onGround = true;
					break;
				}
			}
		}
		else if (HandleFall())
		{
			Log::Info("PlayerController: Falling.");
			onGround = false;
		}

	}

	out::PlayerPositionAndLookPacket response(m_Position,
		m_Yaw * 180.0f / 3.14159f, m_Pitch * 180.0f / 3.14159f, onGround);

	m_Connection->SendPacket(&response);

	UpdateDigging();
}

bool PlayerController::InLoadedChunk() const
{
	return m_World.GetChunk(m_Position) != nullptr;
}

glm::dvec3 PlayerController::GetPosition() const { return m_Position; }
glm::dvec3 PlayerController::GetHeading() const
{
	return glm::dvec3(
		-std::cos(GetPitch()) * std::sin(GetYaw()),
		-std::sin(GetPitch()),
		std::cos(GetPitch()) * std::cos(GetYaw())
	);
}
float PlayerController::GetYaw() const { return m_Yaw; }
float PlayerController::GetPitch() const { return m_Pitch; }

void PlayerController::Move(glm::dvec3 delta)
{
	//delta.y = 0;

	if (m_EntityId != -1)
	{
		PlayerPtr player = m_PlayerManager.GetPlayerByEntityId(m_EntityId);
		if (player)
		{
			EntityPtr entity = player->GetEntity();

			if (entity)
			{
				if (entity->GetVehicleId() != -1)
					return; // Don't move if player is in a vehicle
			}
		}
	}

	m_Position += delta;
}

void PlayerController::SetYaw(float yaw) { m_Yaw = yaw; }
void PlayerController::SetPitch(float pitch) { m_Pitch = pitch; }
void PlayerController::LookAt(glm::dvec3 target)
{
	glm::dvec3 toTarget = target - m_Position;

	double dist = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
	double pitch = -std::atan2(toTarget.y, dist);
	double yaw = -std::atan2(toTarget.x, toTarget.z);

	SetYaw((float)yaw);
	SetPitch((float)pitch);
}