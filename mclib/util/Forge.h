#ifndef MCLIB_UTIL_FORGE_H_
#define MCLIB_UTIL_FORGE_H_

#include <core/Connection.h>
#include <protocol/PacketHandler.h>

class ForgeHandler 
	: public PacketHandler
{
public:
	struct ModInfo
	{
		std::string name;
		std::string version;

		ModInfo(const std::string& name, const std::string& version) : name(name), version(version) {}
	};

private:
	typedef std::unordered_map<std::wstring, std::function<void(const std::string&)>> HandlerMap;

	HandlerMap m_Handlers;
	// Filled out during ping response
	std::vector<ModInfo> m_Mods;
	Connection* m_Connection;
	uint8 m_Version;
	bool m_ModInfoReceived;

	void HandleData(const std::string& data);

	template <typename SerializableData>
	void SendPluginMessage(const std::wstring& channel, SerializableData data)
	{
		std::string out = data.Serialize().ToString();

		out::PluginMessagePacket packet(channel, out);
		m_Connection->SendPacket(&packet);
	}

public:
	MCLIB_API ForgeHandler(PacketDispatcher* dispatcher, Connection* connection);
	MCLIB_API ~ForgeHandler();

	MCLIB_API void SetConnection(Connection* conn) { m_Connection = conn; }

	MCLIB_API void HandlePacket(in::PluginMessagePacket* packet);
	MCLIB_API void HandlePacket(status::in::ResponsePacket* packet);

	MCLIB_API bool HasModInfo() const;
};

#endif
