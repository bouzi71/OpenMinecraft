#ifndef MCLIB_CORE_CLIENT_H
#define MCLIB_CORE_CLIENT_H


#include <core/AuthToken.h>
#include <core/Connection.h>
#include <core/PlayerManager.h>
#include <entity/EntityManager.h>
#include <inventory/Inventory.h>
#include <inventory/Hotbar.h>
#include <network/Network.h>
#include <protocol/PacketDispatcher.h>
#include <util/ObserverSubject.h>
#include <world/World.h>
#include <thread>


class PlayerController;


class ClientListener
{
public:
	virtual void OnTick() = 0;
};


enum class UpdateMethod
{
	CMinecraftBlock,
	Threaded,
	Manual
};


class CMinecraftClient
	: public ObserverSubject<ClientListener>
	, public ConnectionListener
{
public:
	MCLIB_API CMinecraftClient(PacketDispatcher* dispatcher, Version version = Version::Minecraft_1_12_2);
	MCLIB_API ~CMinecraftClient();

	CMinecraftClient(const CMinecraftClient& rhs) = delete;
	CMinecraftClient& operator=(const CMinecraftClient& rhs) = delete;
	CMinecraftClient(CMinecraftClient&& rhs) = delete;
	CMinecraftClient& operator=(CMinecraftClient&& rhs) = delete;

	void MCLIB_API OnSocketStateChange(Socket::Status newState);
	void MCLIB_API UpdateThread();
	void MCLIB_API Update();
	bool MCLIB_API Login(const std::string& host, unsigned short port, const std::string& user, const std::string& password, UpdateMethod method = UpdateMethod::CMinecraftBlock);
	bool MCLIB_API Login(const std::string& host, unsigned short port, const std::string& user, AuthToken token, UpdateMethod method = UpdateMethod::CMinecraftBlock);
	void MCLIB_API Ping(const std::string& host, unsigned short port, UpdateMethod method = UpdateMethod::CMinecraftBlock);

	PacketDispatcher* GetDispatcher() { return m_Dispatcher; }
	Connection* GetConnection() { return &m_Connection; }
	PlayerManager* GetPlayerManager() { return &m_PlayerManager; }
	EntityManager* GetEntityManager() { return &m_EntityManager; }
	InventoryManager* GetInventoryManager() { return m_InventoryManager.get(); }
	Hotbar& GetHotbar() { return m_Hotbar; }
	PlayerController* GetPlayerController() { return m_PlayerController.get(); }
	World* GetWorld() { return &m_World; }

private:
	PacketDispatcher* m_Dispatcher;
	Connection m_Connection;
	EntityManager m_EntityManager;
	PlayerManager m_PlayerManager;
	std::unique_ptr<InventoryManager> m_InventoryManager;
	Hotbar m_Hotbar;
	std::unique_ptr<PlayerController> m_PlayerController;
	World m_World;
	s64 m_LastUpdate;
	bool m_Connected;
	std::thread m_UpdateThread;
};

#endif // CLIENT_H

