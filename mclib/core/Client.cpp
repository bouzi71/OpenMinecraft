#include "stdafx.h"

#include <core/Client.h>

#include <util/Utility.h>

#include <iostream>

// These were changed in MSVC 2015. Redefine them so the old lib files link correctly.
// legacy_stdio_definitions.lib is supposed to define these but it doesn't seem to work.
#if (defined _MSC_VER) && _MSC_VER >= 1900
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;
int (WINAPIV * _fprintf)(FILE* const, char const*, ...) = fprintf;
#endif

CMinecraftClient::CMinecraftClient(PacketDispatcher* dispatcher, Version version)
	: m_Dispatcher(dispatcher),
	m_Connection(m_Dispatcher, version),
	m_EntityManager(m_Dispatcher, version),
	m_PlayerManager(m_Dispatcher, &m_EntityManager),
	m_World(m_Dispatcher),
	m_PlayerController(std::make_unique<PlayerController>(&m_Connection, m_World, m_PlayerManager)),
	m_LastUpdate(0),
	m_Connected(false),
	m_InventoryManager(std::make_unique<InventoryManager>(m_Dispatcher, &m_Connection)),
	m_Hotbar(m_Dispatcher, &m_Connection, m_InventoryManager.get())
{
	m_Connection.RegisterListener(this);
}

CMinecraftClient::~CMinecraftClient()
{
	m_Connection.Disconnect();
	m_Connected = false;
	if (m_UpdateThread.joinable())
		m_UpdateThread.join();
	m_Connection.UnregisterListener(this);
}

void CMinecraftClient::OnSocketStateChange(Socket::Status newState)
{
	m_Connected = (newState == Socket::Status::Connected);
}

void CMinecraftClient::Update()
{
	try
	{
		m_Connection.CreatePacket();
	}
	catch (std::exception& e)
	{
		std::wcout << e.what() << std::endl;
	}

	EntityPtr playerEntity = m_EntityManager.GetPlayerEntity();
	if (playerEntity)
	{
		// Keep entity manager and player controller in sync
		playerEntity->SetPosition(m_PlayerController->GetPosition());
	}

	s64 time = GetTime();
	if (time >= m_LastUpdate + (1000 / 20))
	{
		m_PlayerController->Update();
		NotifyListeners(&ClientListener::OnTick);
		m_LastUpdate = time;
	}
}

void CMinecraftClient::UpdateThread()
{
	while (m_Connected)
	{
		Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool CMinecraftClient::Login(const std::string& host, unsigned short port, const std::string& user, const std::string& password, UpdateMethod method)
{
	if (m_UpdateThread.joinable())
	{
		m_Connected = false;
		m_UpdateThread.join();
	}

	m_LastUpdate = 0;

	if (!m_Connection.Connect(host, port))
		throw std::runtime_error("Could not connect to server");

	if (!m_Connection.Login(user, password))
		return false;

	if (method == UpdateMethod::Threaded)
	{
		m_UpdateThread = std::thread(&CMinecraftClient::UpdateThread, this);
	}
	else if (method == UpdateMethod::CMinecraftBlock)
	{
		UpdateThread();
	}
	return true;
}

bool CMinecraftClient::Login(const std::string& host, unsigned short port, const std::string& user, AuthToken token, UpdateMethod method)
{
	if (m_UpdateThread.joinable())
	{
		m_Connected = false;
		m_UpdateThread.join();
	}

	m_LastUpdate = 0;

	if (!m_Connection.Connect(host, port))
		throw std::runtime_error("Could not connect to server");

	if (!m_Connection.Login(user, token))
		return false;

	if (method == UpdateMethod::Threaded)
	{
		m_UpdateThread = std::thread(&CMinecraftClient::UpdateThread, this);
	}
	else if (method == UpdateMethod::CMinecraftBlock)
	{
		UpdateThread();
	}
	return true;
}

void CMinecraftClient::Ping(const std::string& host, unsigned short port, UpdateMethod method)
{
	if (m_UpdateThread.joinable())
	{
		m_Connected = false;
		m_UpdateThread.join();
	}

	if (!m_Connection.Connect(host, port))
		throw std::runtime_error("Could not connect to server");

	m_Connection.Ping();

	if (method == UpdateMethod::Threaded)
	{
		m_UpdateThread = std::thread(&CMinecraftClient::UpdateThread, this);
	}
	else if (method == UpdateMethod::CMinecraftBlock)
	{
		UpdateThread();
	}
}
