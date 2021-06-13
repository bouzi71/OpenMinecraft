#ifndef MCLIB_UTIL_UTILITY_H
#define MCLIB_UTIL_UTILITY_H

#include <common/AABB.h>
#include <common/JsonFwd.h>
#include <common/Vector.h>
#include <core/AuthToken.h>
#include <core/Client.h>
#include <core/Connection.h>
#include <core/PlayerManager.h>
#include <world/World.h>

#include <fstream>

#include <queue>
#include <utility>

inline std::string to_string(const std::string& str)
{
	return str;
}

inline std::string to_string(const std::wstring& str)
{
	return std::string(str.begin(), str.end());
}

MCLIB_API bool GetProfileToken(const std::string& username, AuthToken* token);
MCLIB_API std::string ParseChatNode(const json& node);
MCLIB_API std::string StripChatMessage(const std::string& message);
MCLIB_API int64 GetTime();


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

class PlayerFollower
	: public PlayerListener
	, public ClientListener
{
private:
	CMinecraftClient* m_Client;
	PlayerManager& m_PlayerManager;
	EntityManager& m_EntityManager;
	PlayerPtr m_Following;
	PlayerController& m_PlayerController;
	std::wstring m_Target;
	uint64 m_LastUpdate;

public:
	MCLIB_API PlayerFollower(PacketDispatcher* dispatcher, CMinecraftClient* client);

	MCLIB_API ~PlayerFollower();

	MCLIB_API void UpdateRotation();

	MCLIB_API void OnTick() override;
	MCLIB_API bool IsIgnored(const std::wstring& name);

	MCLIB_API void FindClosestPlayer();

	MCLIB_API void OnPlayerJoin(PlayerPtr player) override;
	MCLIB_API void OnPlayerLeave(PlayerPtr player) override;
	MCLIB_API void OnPlayerSpawn(PlayerPtr player) override;
	MCLIB_API void OnPlayerDestroy(PlayerPtr player, EntityId eid) override;
	MCLIB_API void OnPlayerMove(PlayerPtr player, glm::dvec3 oldPos, glm::dvec3 newPos) override;
};


/*
class IConsole
{
public:
	virtual void Output(const std::string& str) = 0;
	virtual void Output(const std::wstring& str) = 0;

	virtual IConsole& operator<<(const std::string& str) = 0;
	virtual IConsole& operator<<(const std::wstring& str) = 0;

	template <typename T>
	IConsole& operator<<(T data)
	{
		Output(std::to_string(data));
		return *this;
	}

	IConsole& operator<<(const char* data)
	{
		Output(std::string(data));
		return *this;
	}

	IConsole& operator<<(const wchar_t* data)
	{
		Output(std::wstring(data));
		return *this;
	}
};


class Console
{
private:
	IConsole* m_Impl;

public:
	Console() : m_Impl(nullptr) {}

	// Doesn't take control of impl
	void SetImpl(IConsole* impl)
	{
		m_Impl = impl;
	}

	IConsole* GetImpl() const
	{
		return m_Impl;
	}

	void Output(const std::string& str)
	{
		if (m_Impl)
			m_Impl->Output(str);
	}

	void Output(const std::wstring& str)
	{
		if (m_Impl)
			m_Impl->Output(str);
	}

	template <typename T>
	Console& operator<<(const std::string& str)
	{
		Output(str);
		return *this;
	}

	template <typename T>
	Console& operator<<(const std::wstring& str)
	{
		Output(str);
		return *this;
	}

	template <typename T>
	Console& operator<<(const T& data)
	{
		Output(std::to_string(data));
		return *this;
	}

	Console& operator<<(const char* str)
	{
		Output(std::string(str));
		return *this;
	}

	Console& operator<<(const wchar_t* str)
	{
		Output(std::wstring(str));
		return *this;
	}
};

class LoggerConsole 
	: public IConsole
{
private:
	std::ofstream m_Out;
	std::string m_Filename;

public:
	LoggerConsole(const std::string& filename)
	{
		m_Out.open(filename.c_str(), std::ios::out);
	}

	~LoggerConsole()
	{
		m_Out.close();
	}

	void Output(const std::string& str) override
	{
		m_Out << str << std::endl;
	}

	void Output(const std::wstring& str) override
	{
		m_Out << std::string(str.begin(), str.end()) << std::endl;
	}

	IConsole& operator<<(const std::string& str) override
	{
		Output(str);
		return *this;
	}
	IConsole& operator<<(const std::wstring& str) override
	{
		Output(str);
		return *this;
	}
};
*/

#endif // UTILITY_H
