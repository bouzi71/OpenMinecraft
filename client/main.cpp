#include "Logger.h"

#include <mclib/common/Common.h>
#include <mclib/core/Client.h>
#include <mclib/util/Forge.h>
#include <mclib/util/Hash.h>
#include <mclib/util/Utility.h>
#include <mclib/util/VersionFetcher.h>

#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "../Debug/mclibd.lib")
#else
#pragma comment(lib, "../Release/mclib.lib")
#endif

namespace
{

const std::string server("127.0.0.1");
const u16 port = 25565;
const std::string username("Bouzi71");
const std::string password("");
const bool useProfileToken = false;

} // ns

int run(Version versions, ForgeHandler& forge);

int main(void)
{
	VersionFetcher versionFetcher(server, port);

	std::cout << "Fetching version" << std::endl;

	auto version = versionFetcher.GetVersion();

	BlockRegistry::GetInstance()->RegisterVanillaBlocks(version);

	std::cout << "Connecting with version " << to_string(version) << std::endl;
	return run(version, versionFetcher.GetForge());
}

int run(Version version, ForgeHandler& forge)
{
	PacketDispatcher dispatcher;
	CMinecraftClient client(&dispatcher, version);

	forge.SetConnection(client.GetConnection());

	client.GetPlayerController()->SetHandleFall(true);
	client.GetConnection()->GetSettings().SetMainHand(MainHand::Right).SetViewDistance(16);

	example::Logger logger(&client, &dispatcher);

	try
	{
		std::cout << "Logging in." << std::endl;

		AuthToken token;

		if (useProfileToken && GetProfileToken(username, &token))
		{
			std::cout << "Using profile token." << std::endl;
			client.Login(server, port, username, token, UpdateMethod::Block);
		}
		else
		{
			client.Login(server, port, username, password, UpdateMethod::Block);
		}
	}
	catch (std::exception& e)
	{
		std::wcout << e.what() << std::endl;
		return 1;
	}

	return 0;
}
