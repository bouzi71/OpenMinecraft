#ifndef TERRACOTTA_GAME_H_
#define TERRACOTTA_GAME_H_

#if 0

#include <core/Client.h>

#include "Camera.h"
#include "Transform.h"
#include "Collision.h"

#include <protocol/packets/Packet.h>
#include <protocol/PacketHandler.h>

#include <world/World.h>



namespace terra
{



	class Game 
		: public PacketHandler
		, public PlayerListener
	{
	public:
		Game(PacketDispatcher* dispatcher, const std::shared_ptr<ICameraComponent3D>& camera);

		void OnMouseChange(double x, double y);
		void OnMouseScroll(double offset_x, double offset_y);
		void OnMousePress(int button, int action, int mods);

		// PlayerListener
		void OnClientSpawn(PlayerPtr player);

		// PacketHandler
		void HandlePacket(in::UpdateHealthPacket* packet);
		void HandlePacket(in::EntityVelocityPacket* packet);
		void HandlePacket(in::SpawnPositionPacket* packet);

		void Update();
		void UpdateClient();

		void CreatePlayer(World* world);

		std::shared_ptr<ICameraComponent3D> GetCamera() { return m_Camera; }
		glm::dvec3 GetPosition();
		CMinecraftClient& GetNetworkClient() { return m_NetworkClient; }

	private:
		CMinecraftClient m_NetworkClient;
		std::unique_ptr<Player> m_Player;
		std::shared_ptr<ICameraComponent3D> m_Camera;
		float m_DeltaTime;
		float m_LastFrame;
		float m_LastPositionTime;
		bool m_Sprinting;
	};

} // ns terra

#endif

#endif