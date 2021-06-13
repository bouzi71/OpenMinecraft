#pragma once

#include "Camera.h"

#include <core/Client.h>

#include "Camera.h"
#include "Transform.h"
#include "Collision.h"

#include <protocol/packets/Packet.h>
#include <protocol/PacketHandler.h>

#include <world/World.h>

#include "render/ChunkMesh.h"
#include "render/ChunkMeshGenerator.h"


class CGamePlayer
{
public:
	CGamePlayer(World* world);

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





class CSceneMinecraft
	: public SceneBase
	, public PacketHandler
	, public PlayerListener
{
public:
	CSceneMinecraft(IBaseManager& BaseManager, IRenderWindow& RenderWindow);
	virtual ~CSceneMinecraft();

	std::shared_ptr<terra::render::ChunkMeshGenerator> GetMeshGen();

	// PlayerListener
	void OnClientSpawn(PlayerPtr player);

	// PacketHandler
	void HandlePacket(in::UpdateHealthPacket* packet);
	void HandlePacket(in::EntityVelocityPacket* packet);
	void HandlePacket(in::SpawnPositionPacket* packet);


	// IGameState
	void Initialize() override;
	void Finalize() override;

	virtual void OnUpdate(UpdateEventArgs& e) override;

	virtual bool OnMousePressed(const MouseButtonEventArgs & e, const Ray& RayToWorld) override;
	virtual void OnMouseMoved(const MouseMotionEventArgs & e, const Ray& RayToWorld) override;

	// Keyboard events
	virtual bool OnWindowKeyPressed(KeyEventArgs& e) override;
	virtual void OnWindowKeyReleased(KeyEventArgs& e) override;


private:
	void Update(UpdateEventArgs& e);
	void UpdateClient();

	void CreatePlayer(World* world);
	glm::dvec3 GetPosition();
	CMinecraftClient& GetNetworkClient() { return m_NetworkClient; }


private:
	PacketDispatcher dispatcher;
	CMinecraftClient m_NetworkClient;
	World world;

	std::shared_ptr<terra::render::ChunkMeshGenerator> m_MeshGen;

private:
	
	std::unique_ptr<CGamePlayer> m_Player;
	float m_DeltaTime;
	float m_LastFrame;
	float m_LastPositionTime;
	bool m_Sprinting;



};
