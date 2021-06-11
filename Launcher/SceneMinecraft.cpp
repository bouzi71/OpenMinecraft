#include "stdafx.h"

// Gerenal
#include "SceneMinecraft.h"

// Additional
#include "Passes/MinecraftCubePass.h"

#include <iostream>
#include <iterator>
#include <fstream>

#include <core/Client.h>
#include <util/Utility.h>

#include "ChatWindow.h"
#include "math/TypeUtil.h"

#include "render/ChunkMesh.h"
#include "render/ChunkMeshGenerator.h"

#include "math/Plane.h"
#include "math/TypeUtil.h"

#include <util/Utility.h>
#include <util/Utility.h>
#include <world/World.h>
#include <common/AABB.h>
#include <inventory/Inventory.h>
#include <protocol/packets/Packet.h>


#include "assets/AssetCache.h"
#include "assets/AssetLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "assets/stb_image.h"


#ifndef M_TAU
#define M_TAU ((3.14159f) * (2.0f))
#endif


std::unique_ptr<terra::AssetCache> g_AssetCache;

void GetConnectionParams()
{
	std::string server = "127.0.0.1";
	uint16 port = 25565;
	std::string username = "Bouzi71";
	std::string password = "";

	std::ifstream config_file("config.json");

	if (config_file.is_open())
	{
		json config_root;

		try
		{
			config_root = json::parse(config_file);
		}
		catch (json::parse_error& e)
		{
			std::cout << e.what() << std::endl;
			return;
		}

		json login_node = config_root.value("login", json());
		if (login_node.is_object())
		{
			username = login_node.value("username", "");
			password = login_node.value("password", "");
			server = login_node.value("server", "");
			port = login_node.value("port", static_cast<uint16>(25565));
		}
	}
}








CGamePlayer::CGamePlayer(World* world) : m_CollisionDetector(world), m_OnGround(false), m_Sneaking(false), m_Transform({})
{
	m_Transform.bounding_box = CMinecraftAABB(glm::dvec3(-0.3, 0, -0.3), glm::dvec3(0.3, 1.8, 0.3));
	m_Transform.max_speed = 14.3f;
}

bool CGamePlayer::OnGround()
{
	return m_OnGround;
}

void CGamePlayer::Update(float dt)
{
	const float kMaxAcceleration = 100.0f;
	const double kEpsilon = 0.0001;

	using vec3 = glm::dvec3;

	vec3 horizontal_acceleration = m_Transform.acceleration;
	horizontal_acceleration.y = 0;
	horizontal_acceleration = Truncate(horizontal_acceleration, kMaxAcceleration);

	glm::vec3 acceleration(horizontal_acceleration.x, -38 + m_Transform.acceleration.y, horizontal_acceleration.z);

	m_OnGround = false;

	m_CollisionDetector.ResolveCollisions(&m_Transform, dt, &m_OnGround);

	m_Transform.velocity += (acceleration * dt);
	m_Transform.input_velocity += (glm::vec3(m_Transform.input_acceleration) * dt);
	m_Transform.orientation += m_Transform.rotation * dt;

	if (glm::length2(m_Transform.velocity) < kEpsilon) 
		m_Transform.velocity = vec3(0, 0, 0);

	if (glm::length2(m_Transform.input_velocity) < kEpsilon)
		m_Transform.input_velocity = vec3(0, 0, 0);

	float drag = 0.98 - (int)m_OnGround * 0.13;
	m_Transform.velocity *= drag;
	m_Transform.input_velocity *= drag;

	m_Transform.orientation = glm::clamp(m_Transform.orientation, -M_TAU, M_TAU);

	vec3 horizontal_velocity = m_Transform.input_velocity;
	horizontal_velocity.y = 0;
	horizontal_velocity = Truncate(horizontal_velocity, m_Transform.max_speed);

	m_Transform.input_velocity = horizontal_velocity + vec3(0, m_Transform.input_velocity.y, 0);

	m_Transform.acceleration = vec3();
	m_Transform.input_acceleration = vec3();
	m_Transform.rotation = 0.0f;
}
















CSceneMinecraft::CSceneMinecraft(IBaseManager& BaseManager, IRenderWindow& RenderWindow)
	: SceneBase(BaseManager, RenderWindow)
	, PacketHandler(&dispatcher)
	, m_NetworkClient(&dispatcher, Version::Minecraft_1_12_2)
	, world(GetNetworkClient().GetDispatcher())
	, m_Sprinting(false)
	, m_LastPositionTime(0)
{
	m_NetworkClient.GetPlayerController()->SetHandleFall(true);
	m_NetworkClient.GetConnection()->GetSettings().SetMainHand(MainHand::Right).SetViewDistance(4);

	m_NetworkClient.GetPlayerManager()->RegisterListener(this);

	dispatcher.RegisterHandler(State::Play, play::UpdateHealth, this);
	dispatcher.RegisterHandler(State::Play, play::EntityVelocity, this);
	dispatcher.RegisterHandler(State::Play, play::SpawnPosition, this);
}

CSceneMinecraft::~CSceneMinecraft()
{
	Log::Info("Scene destroyed.");
}

std::shared_ptr<terra::render::ChunkMeshGenerator> CSceneMinecraft::GetMeshGen()
{
	return m_MeshGen;
}




void CSceneMinecraft::OnClientSpawn(PlayerPtr player)
{
	m_Player->GetTransform().position = player->GetEntity()->GetPosition();
	m_Player->GetTransform().velocity = glm::dvec3();
	m_Player->GetTransform().acceleration = glm::dvec3();
	m_Player->GetTransform().orientation = player->GetEntity()->GetYaw() * 3.14159f / 180.0f;
}

void CSceneMinecraft::HandlePacket(in::UpdateHealthPacket* packet)
{

}

void CSceneMinecraft::HandlePacket(in::EntityVelocityPacket* packet)
{
	EntityId eid = packet->GetEntityId();

	auto playerEntity = m_NetworkClient.GetEntityManager()->GetPlayerEntity();
	if (playerEntity && playerEntity->GetEntityId() == eid)
	{
		glm::dvec3 newVelocity = glm::dvec3(packet->GetVelocity()) * 20.0 / 8000.0;

		std::cout << "Applying new velocity " << newVelocity << std::endl;
		m_Player->GetTransform().velocity = newVelocity;
	}
}

void CSceneMinecraft::HandlePacket(in::SpawnPositionPacket* packet)
{
	int64 x = packet->GetLocation().GetX();
	int64 y = packet->GetLocation().GetY();
	int64 z = packet->GetLocation().GetZ();
}






//
// IGameState
//
void CSceneMinecraft::Initialize()
{
	__super::Initialize();

	// Light
	{
		auto lightNode = CreateSceneNodeT<ISceneNode>();
		lightNode->SetName("Light");
		lightNode->SetLocalPosition(glm::vec3(-50.0f, 250.0f, -50.0f));
		lightNode->SetLocalRotationDirection(glm::vec3(-0.5, -0.5f, -0.5f));
		//lightNode->SetLocalRotationEuler(glm::vec3(45.0f, -45.0f, 0.0f));

		auto lightComponent = GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<CLightComponent>(cSceneNodeLightComponent, *lightNode.get());
		lightComponent->SetLight(MakeShared(CLight, GetBaseManager()));
		lightComponent->GetLight()->SetCastShadows(true);
		lightComponent->GetLight()->SetType(ELightType::Spot);
		lightComponent->GetLight()->SetAmbientColor(ColorRGB(0.25f));
		lightComponent->GetLight()->SetColor(ColorRGB(1.0f, 1.0f, 1.0f));
		lightComponent->GetLight()->SetRange(1000.0f);
		lightComponent->GetLight()->SetIntensity(1.0077f);
		lightComponent->GetLight()->SetSpotlightAngle(30.0f);

		lightNode->AddComponent(cSceneNodeLightComponent, lightComponent);
	}

	// Camera
	{
		auto cameraNode = CreateSceneNodeT<ISceneNode>();
		cameraNode->SetName("Camera");
		cameraNode->AddComponentT(GetBaseManager().GetManager<IObjectsFactory>()->GetClassFactoryCast<IComponentFactory>()->CreateComponentT<ICameraComponent3D>(cSceneNodeCameraComponent, *cameraNode));

		SetCameraController(MakeShared(CFreeCameraController));
		GetCameraController()->SetCamera(cameraNode->GetComponentT<ICameraComponent3D>());
		GetCameraController()->GetCamera()->SetPerspectiveProjection(75.0f, static_cast<float>(GetRenderWindow().GetWindowWidth()) / static_cast<float>(GetRenderWindow().GetWindowHeight()), 1.0f, 500.0f);
		GetCameraController()->GetCamera()->SetPosition(glm::vec3(0.0f));
		GetCameraController()->GetCamera()->SetYaw(225);
		GetCameraController()->GetCamera()->SetPitch(-45);
	}

	std::shared_ptr<CMinecraftCubePass> pass = MakeShared(CMinecraftCubePass, GetRenderDevice(), *this);
	pass->ConfigurePipeline(GetRenderWindow().GetRenderTarget());

	GetRenderer()->Add3DPass(pass);




	BlockRegistry::GetInstance()->RegisterVanillaBlocks();

	std::string server = "127.0.0.1";
	uint16 port = 25565;
	std::string username = "Bouzi71";
	std::string password = "";

	g_AssetCache = std::make_unique<terra::AssetCache>(GetBaseManager());

	terra::AssetLoader asset_loader(*g_AssetCache);
	if (false == asset_loader.LoadArchive("1.12.2.jar"))
	{
		std::cout << "Failed to load assets." << std::endl;
		return;
	}

	try
	{
		std::cout << "Logging in." << std::endl;
		if (false == GetNetworkClient().Login(server, port, username, password, UpdateMethod::Manual))
		{
			std::cout << "Failed to login." << std::endl;
			return;
		}

		Sleep(1000);
	}
	catch (std::exception& e)
	{
		std::wcout << e.what() << std::endl;
		return;
	}

	m_MeshGen = std::make_shared<terra::render::ChunkMeshGenerator>(GetRenderDevice(), &world, GetCameraController()->GetCamera()->GetPosition());

	terra::ChatWindow chat(GetNetworkClient().GetDispatcher(), GetNetworkClient().GetConnection());

	CreatePlayer(&world);

}

void CSceneMinecraft::Finalize()
{
	SceneBase::Finalize();
}

void CSceneMinecraft::OnUpdate(UpdateEventArgs & e)
{
	__super::OnUpdate(e);

	Update(e);
}

bool CSceneMinecraft::OnMousePressed(const MouseButtonEventArgs & e, const Ray& RayToWorld)
{
	return false;
}

void CSceneMinecraft::OnMouseMoved(const MouseMotionEventArgs & e, const Ray & RayToWorld)
{
}



//
// Keyboard events
//
bool CSceneMinecraft::OnWindowKeyPressed(KeyEventArgs & e)
{
	/*if (e.Key == KeyCode::W)
	{
		direction += front;

		if (e.Control)
		{
			m_Sprinting = true;
		}
		return true;
	}
	else if (e.Key == KeyCode::S)
	{
		direction -= front;
		m_Sprinting = false;
		return true;
	}
	else if (e.Key == KeyCode::A)
	{
		glm::dvec3 right = Vector3Normalize(front.Cross(glm::dvec3(0, 1, 0)));

		direction -= right;
		return true;
	}
	else if (e.Key == KeyCode::D)
	{
		glm::dvec3 right = Vector3Normalize(front.Cross(glm::dvec3(0, 1, 0)));

		direction += right;
		return true;
	}
	else */if (e.Key == KeyCode::Space)
	{
		if (m_Player->OnGround())
		{
			m_Player->GetTransform().input_acceleration += glm::dvec3(0, 6 / m_DeltaTime, 0);
		}
	}

	return SceneBase::OnWindowKeyPressed(e);
}

void CSceneMinecraft::OnWindowKeyReleased(KeyEventArgs & e)
{
	SceneBase::OnWindowKeyReleased(e);
}




//
// Private
//
void CSceneMinecraft::Update(UpdateEventArgs& e)
{
	UpdateClient();

	if (GetNetworkClient().GetConnection()->GetProtocolState() != State::Play)
		return;

	float current_frame = (e.TotalTime / 1000.0f);
	if (current_frame - m_LastFrame < 1.0f / 60.0f)
		return;

	m_DeltaTime = current_frame - m_LastFrame;
	m_LastFrame = current_frame;

	glm::dvec3 front(
		std::cos(glm::radians(GetCameraController()->GetCamera()->GetYaw())) * std::cos(0),
		std::sin(0),
		std::sin(glm::radians(GetCameraController()->GetCamera()->GetYaw())) * std::cos(0)
	);

	glm::dvec3 direction;

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
			glm::dvec3 right = Vector3Normalize(front.Cross(glm::dvec3(0, 1, 0)));

			direction -= right;
		}

		if (m_Window->IsKeyDown(GLFW_KEY_D))
		{
			glm::dvec3 right = Vector3Normalize(front.Cross(glm::dvec3(0, 1, 0)));

			direction += right;
		}
	}*/

	if (false == m_Player->OnGround())
	{
		direction *= 0.2;
	}

	if (m_Sprinting)
	{
		if (glm::length2(direction) <= 0.0)
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
			m_Player->GetTransform().input_acceleration += glm::dvec3(0, 6 / m_DeltaTime, 0);
		}
	}*/

	m_Player->GetTransform().max_speed = 4.3f + (int)m_Sprinting * 1.3f;
	m_Player->GetTransform().input_acceleration += glm::vec3(direction) * 85.0f;

	m_Player->Update(m_DeltaTime);

	glm::vec3 eye = VecToGLM(m_Player->GetTransform().position) + glm::vec3(0, 1.6, 0);
	//GetCameraController()->GetCamera()->SetPosition(eye);

	constexpr float kTickTime = 1000.0f / 20.0f / 1000.0f;

	if ((current_frame > m_LastPositionTime + kTickTime) && (m_NetworkClient.GetConnection()->GetProtocolState() == State::Play))
	{
		float yaw = glm::radians(GetCameraController()->GetCamera()->GetYaw()) - glm::radians(90.0f);
		float pitch = -(glm::radians(GetCameraController()->GetCamera()->GetPitch()));

		//out::PlayerPositionAndLookPacket response(m_Player->GetTransform().position, yaw * 180.0f / 3.14159f, pitch * 180.0f / 3.14159f, m_Player->OnGround());
		//m_NetworkClient.GetConnection()->SendPacket(&response);

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

	m_MeshGen->ProcessChunks();
}

void CSceneMinecraft::UpdateClient()
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

void CSceneMinecraft::CreatePlayer(World* world)
{
	m_Player = std::make_unique<CGamePlayer>(world);
}

glm::dvec3 CSceneMinecraft::GetPosition()
{
	return m_Player->GetTransform().position;
}