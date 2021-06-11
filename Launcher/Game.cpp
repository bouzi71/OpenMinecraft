#include "stdafx.h"

#if 0

#include "Game.h"

#include <util/Utility.h>
#include <util/Utility.h>
#include <world/World.h>
#include <common/AABB.h>
#include <inventory/Inventory.h>
#include <protocol/packets/Packet.h>

#include "math/Plane.h"
#include "math/TypeUtil.h"

#include <iostream>
#include <limits>

namespace terra
{

#ifndef M_TAU
	#define M_TAU 3.14159 * 2
#endif

	float clamp(float value, float min, float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	

	Game::Game(PacketDispatcher* dispatcher, const std::shared_ptr<ICameraComponent3D>& camera)
		: PacketHandler(dispatcher),
		m_NetworkClient(dispatcher, Version::Minecraft_1_12_2),
		m_Camera(camera),
		m_Sprinting(false),
		m_LastPositionTime(0)
	{
		/*if (window)
		{
			window->RegisterMouseChange(std::bind(&Game::OnMouseChange, this, std::placeholders::_1, std::placeholders::_2));
			window->RegisterMouseScroll(std::bind(&Game::OnMouseScroll, this, std::placeholders::_1, std::placeholders::_2));
			window->RegisterMouseButton(std::bind(&Game::OnMousePress, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		}*/

		m_NetworkClient.GetPlayerController()->SetHandleFall(true);
		m_NetworkClient.GetConnection()->GetSettings().SetMainHand(MainHand::Right).SetViewDistance(4);

		m_NetworkClient.GetPlayerManager()->RegisterListener(this);

		dispatcher->RegisterHandler(State::Play, play::UpdateHealth, this);
		dispatcher->RegisterHandler(State::Play, play::EntityVelocity, this);
		dispatcher->RegisterHandler(State::Play, play::SpawnPosition, this);
	}

	void Game::CreatePlayer(World* world)
	{
		m_Player = std::make_unique<terra::Player>(world);
	}

	Vector3d Game::GetPosition()
	{
		return m_Player->GetTransform().position;
	}

	void Game::Update()
	{
		UpdateClient();

		if (GetNetworkClient().GetConnection()->GetProtocolState() != State::Play)
			return;

		float current_frame = /*(float)glfwGetTime()*/1231;
		if (current_frame - m_LastFrame < 1.0f / 60.0f)
			return;

		m_DeltaTime = current_frame - m_LastFrame;
		m_LastFrame = current_frame;

		Vector3d front(
			std::cos(m_Camera->GetYaw()) * std::cos(0),
			std::sin(0),
			std::sin(m_Camera->GetYaw()) * std::cos(0)
		);

		Vector3d direction;

		/*if (m_Window)
		{

			if (m_Window->IsKeyDown(GLFW_KEY_W))
			{
				direction += front;

				if (m_Window->IsKeyDown(GLFW_KEY_LEFT_CONTROL))
				{
					m_Sprinting = true;
				}
			}

			if (m_Window->IsKeyDown(GLFW_KEY_S))
			{
				direction -= front;
				m_Sprinting = false;
			}

			if (m_Window->IsKeyDown(GLFW_KEY_A))
			{
				Vector3d right = Vector3Normalize(front.Cross(Vector3d(0, 1, 0)));

				direction -= right;
			}

			if (m_Window->IsKeyDown(GLFW_KEY_D))
			{
				Vector3d right = Vector3Normalize(front.Cross(Vector3d(0, 1, 0)));

				direction += right;
			}
		}*/

		if (!m_Player->OnGround())
		{
			direction *= 0.2;
		}

		if (m_Sprinting)
		{
			if (direction.LengthSq() <= 0.0)
			{
				m_Sprinting = false;
			}
			else
			{
				direction *= 1.3f;
			}
			// TODO BOUZI FOV
			//m_Camera.SetFov(glm::radians(90.0f));
		}
		else
		{
			// TODO BOUZI FOV
			//m_Camera.SetFov(glm::radians(80.0f));
		}

		/*if (m_Window)
		{
			if (m_Window->IsKeyDown(GLFW_KEY_SPACE) && m_Player->OnGround())
			{
				m_Player->GetTransform().input_acceleration += Vector3d(0, 6 / m_DeltaTime, 0);
			}
		}*/

		m_Player->GetTransform().max_speed = 4.3f + (int)m_Sprinting * 1.3f;
		m_Player->GetTransform().input_acceleration += direction * 85.0f;

		m_Player->Update(m_DeltaTime);

		glm::vec3 eye = math::VecToGLM(m_Player->GetTransform().position) + glm::vec3(0, 1.6, 0);
		m_Camera->SetPosition(eye);

		constexpr float kTickTime = 1000.0f / 20.0f / 1000.0f;

		if (current_frame > m_LastPositionTime + kTickTime && m_NetworkClient.GetConnection()->GetProtocolState() == State::Play)
		{
			float yaw = glm::radians(m_Camera->GetYaw()) - glm::radians(90.0f);
			float pitch = -(m_Camera->GetPitch());

			out::PlayerPositionAndLookPacket response(m_Player->GetTransform().position, yaw * 180.0f / 3.14159f, pitch * 180.0f / 3.14159f, m_Player->OnGround());
			m_NetworkClient.GetConnection()->SendPacket(&response);

			m_LastPositionTime = current_frame;

			/*if (m_Player->IsSneaking() && m_Window && !m_Window->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
			{
				out::EntityActionPacket packet(0, out::EntityActionPacket::Action::StopSneak);
				m_NetworkClient.GetConnection()->SendPacket(&packet);

				m_Player->SetSneaking(false);
			}
			else if (!m_Player->IsSneaking() && m_Window && m_Window->IsKeyDown(GLFW_KEY_LEFT_SHIFT))
			{
				out::EntityActionPacket packet(0, out::EntityActionPacket::Action::StartSneak);
				m_NetworkClient.GetConnection()->SendPacket(&packet);

				m_Player->SetSneaking(true);
			}*/
		}
	}

	void Game::UpdateClient()
	{
		try
		{
			m_NetworkClient.GetConnection()->CreatePacket();
		}
		catch (std::exception& e)
		{
			std::wcout << e.what() << std::endl;
		}
	}

	void Game::OnMouseChange(double offset_x, double offset_y)
	{
		//m_Camera.ProcessRotation((float)offset_x, (float)offset_y);
	}

	void Game::OnMouseScroll(double offset_x, double offset_y)
	{
		//m_Camera.ProcessZoom((float)offset_y);
	}

	void Game::OnClientSpawn(PlayerPtr player)
	{
		m_Player->GetTransform().position = player->GetEntity()->GetPosition();
		m_Player->GetTransform().velocity = Vector3d();
		m_Player->GetTransform().acceleration = Vector3d();
		m_Player->GetTransform().orientation = player->GetEntity()->GetYaw() * 3.14159f / 180.0f;
	}

	void Game::HandlePacket(in::UpdateHealthPacket* packet)
	{

	}

	void Game::HandlePacket(in::EntityVelocityPacket* packet)
	{
		EntityId eid = packet->GetEntityId();

		auto playerEntity = m_NetworkClient.GetEntityManager()->GetPlayerEntity();
		if (playerEntity && playerEntity->GetEntityId() == eid)
		{
			Vector3d newVelocity = ToVector3d(packet->GetVelocity()) * 20.0 / 8000.0;

			std::cout << "Applying new velocity " << newVelocity << std::endl;
			m_Player->GetTransform().velocity = newVelocity;
		}
	}

	void Game::HandlePacket(in::SpawnPositionPacket* packet)
	{
		s64 x = packet->GetLocation().GetX();
		s64 y = packet->GetLocation().GetY();
		s64 z = packet->GetLocation().GetZ();
	}

	// TODO: Temporary fun code
	template <typename T>
	inline T Sign(T val)
	{
		return std::signbit(val) ? static_cast<T>(-1) : static_cast<T>(1);
	}

	// TODO: Temporary fun code
	inline Vector3d BasisAxis(int basisIndex)
	{
		static const Vector3d axes[3] = { Vector3d(1, 0, 0), Vector3d(0, 1, 0), Vector3d(0, 0, 1) };
		return axes[basisIndex];
	}

	// TODO: Temporary fun code
	std::pair<Vector3d, Face> GetClosestNormal(const Vector3d& pos, CMinecraftAABB bounds)
	{
		Vector3d center = bounds.min + (bounds.max - bounds.min) / 2;
		Vector3d dim = bounds.max - bounds.min;
		Vector3d offset = pos - center;

		double minDist = std::numeric_limits<double>::max();
		Vector3d normal;

		for (int i = 0; i < 3; ++i)
		{
			double dist = dim[i] - std::abs(offset[i]);
			if (dist < minDist)
			{
				minDist = dist;
				normal = BasisAxis(i) * Sign(offset[i]);
			}
		}

		Face face = Face::North;

		if (normal.x == 1)
		{
			face = Face::East;
		}
		else if (normal.x == -1)
		{
			face = Face::West;
		}
		else if (normal.y == 1)
		{
			face = Face::Top;
		}
		else if (normal.y == -1)
		{
			face = Face::Bottom;
		}
		else if (normal.z == 1)
		{
			face = Face::South;
		}

		return std::make_pair<>(normal, face);
	}

	// TODO: Temporary fun code
	bool RayCast(World& world, Vector3d from, Vector3d direction, double range, Vector3d& hit, Vector3d& normal, Face& face)
	{
		static const std::vector<Vector3d> directions = {
			Vector3d(0, 0, 0),
			Vector3d(1, 0, 0), Vector3d(-1, 0, 0),
			Vector3d(0, 1, 0), Vector3d(0, -1, 0),
			Vector3d(0, 0, 1), Vector3d(0, 0, -1),
			Vector3d(0, 1, 1), Vector3d(0, 1, -1),
			Vector3d(1, 1, 0), Vector3d(1, 1, 1), Vector3d(1, 1, -1),
			Vector3d(-1, 1, 0), Vector3d(-1, 1, 1), Vector3d(-1, 1, -1),

			Vector3d(0, -1, 1), Vector3d(0, -1, -1),
			Vector3d(1, -1, 0), Vector3d(1, -1, 1), Vector3d(1, -1, -1),
			Vector3d(-1, -1, 0), Vector3d(-1, -1, 1), Vector3d(-1, -1, -1)
		};

		CMinecraftRay ray(from, direction);

		double closest_distance = std::numeric_limits<double>::max();
		Vector3d closest_pos;
		CMinecraftAABB closest_aabb;
		bool collided = false;

		for (double i = 0; i < range + 1; ++i)
		{
			Vector3d position = from + direction * i;

			for (Vector3d checkDirection : directions)
			{
				Vector3d checkPos = position + checkDirection;
				const CMinecraftBlock* block = world.GetBlock(checkPos);

				if (block && block->IsOpaque())
				{
					CMinecraftAABB bounds = block->GetBoundingBox(checkPos);
					double distance;

					if (bounds.Intersects(ray, &distance))
					{
						if (distance < 0 || distance > range) continue;

						if (distance < closest_distance)
						{
							closest_distance = distance;
							closest_pos = from + direction * closest_distance;
							closest_aabb = bounds;
							collided = true;
						}
					}
				}
			}
		}

		if (collided)
		{
			hit = closest_pos;
			std::tie(normal, face) = GetClosestNormal(hit, closest_aabb);
			return true;
		}

		return false;
	}

	// TODO: Temporary fun code
	void Game::OnMousePress(int button, int action, int mods)
	{
		/*if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		{
			using namespace out;

			auto& world = *m_NetworkClient.GetWorld();
			Vector3d position(m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z);
			Vector3d forward(m_Camera.GetFront().x, m_Camera.GetFront().y, m_Camera.GetFront().z);
			Vector3d hit;
			Vector3d normal;
			Face face;

			if (RayCast(world, position, forward, 5.0, hit, normal, face))
			{

				{
					PlayerDiggingPacket::Status status = PlayerDiggingPacket::Status::StartedDigging;
					PlayerDiggingPacket packet(status, ToVector3i(hit + forward * 0.1), Face::West);

					m_NetworkClient.GetConnection()->SendPacket(&packet);
				}

				{
					PlayerDiggingPacket::Status status = PlayerDiggingPacket::Status::FinishedDigging;
					PlayerDiggingPacket packet(status, ToVector3i(hit + forward * 0.1), Face::West);

					m_NetworkClient.GetConnection()->SendPacket(&packet);
				}
			}

			AnimationPacket animation;
			m_NetworkClient.GetConnection()->SendPacket(&animation);

		}
		else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
		{
			auto& world = *m_NetworkClient.GetWorld();
			Vector3d position(m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z);
			Vector3d forward(m_Camera.GetFront().x, m_Camera.GetFront().y, m_Camera.GetFront().z);
			Vector3d hit;
			Vector3d normal;
			Face face;

			if (RayCast(world, position, forward, 5.0, hit, normal, face))
			{
				Inventory& inventory = *m_NetworkClient.GetInventoryManager()->GetPlayerInventory();

				auto& hotbar = m_NetworkClient.GetHotbar();
				s32 slot_id = hotbar.GetSelectedSlot() + Inventory::HOTBAR_SLOT_START;

				Slot slot = inventory.GetItem(slot_id);

				// Item ids are separate from block ids.
				const u32 draw_block = 9;

				if (slot.GetItemId() != draw_block)
				{
					out::CreativeInventoryActionPacket packet(slot_id, Slot(draw_block, 1, 0));
					m_NetworkClient.GetConnection()->SendPacket(&packet);
				}

				Vector3i target = ToVector3i(hit + forward * 0.1);

				out::PlayerBlockPlacementPacket packet(target, face, Hand::Main, Vector3f(1.0f, 0.0f, 0.0f));
				m_NetworkClient.GetConnection()->SendPacket(&packet);
			}
		}*/
	}

} // ns terra

#endif