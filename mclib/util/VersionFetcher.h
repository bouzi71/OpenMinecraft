#ifndef _MCLIB_UTIL_VERSIONFETCHER_H_
#define _MCLIB_UTIL_VERSIONFETCHER_H_

#include <common/JsonFwd.h>
#include <core/Client.h>
#include <util/Forge.h>

class VersionFetcher 
	: public ConnectionListener
{
public:
	MCLIB_API VersionFetcher(const std::string& host, u16 port);

	MCLIB_API Version GetVersion();

	// ConnectionListener
	MCLIB_API void OnPingResponse(const json& node) override;

	ForgeHandler& GetForge() { return m_Forge; }

private:
	PacketDispatcher m_Dispatcher;
	Version m_Version;
	ForgeHandler m_Forge;
	Connection* m_Connection;
	std::string m_Host;
	u16 m_Port;
	bool m_Found;
};

#endif
