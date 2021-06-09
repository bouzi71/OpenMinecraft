#ifndef TERRACOTTA_GAME_H_
#define TERRACOTTA_GAME_H_

#include <mclib/core/Client.h>

#include "Camera.h"
#include "GameWindow.h"
#include "Transform.h"
#include "Collision.h"

#include <mclib/protocol/packets/Packet.h>
#include <mclib/protocol/PacketHandler.h>
#include <mclib/world/World.h>



namespace terra
{
	class Player
	{
	public:
		Player(World* world);

		void Update(float dt);

		Transform& GetTransform() noexcept { return m_Transform; }

		bool OnGround();
		bool IsSneaking() const { return m_Sneaking; }
		void SetSneaking(bool sneaking) { m_Sneaking = sneaking; }

		CollisionDetector& GetCollisionDetector() { return m_CollisionDetector; }
	private:
		bool m_OnGround;
		bool m_Sneaking;
		Transform m_Transform;
		CollisionDetector m_CollisionDetector;
	};


	class Game 
		: public PacketHandler
		, public PlayerListener
	{
	public:
		Game(PacketDispatcher* dispatcher, GameWindow& window, const Camera& camera);

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

		Camera& GetCamera() { return m_Camera; }
		Vector3d GetPosition();
		CMinecraftClient& GetNetworkClient() { return m_NetworkClient; }

	private:
		CMinecraftClient m_NetworkClient;
		GameWindow& m_Window;
		std::unique_ptr<Player> m_Player;
		Camera m_Camera;
		float m_DeltaTime;
		float m_LastFrame;
		float m_LastPositionTime;
		bool m_Sprinting;
	};

} // ns terra

#endif
