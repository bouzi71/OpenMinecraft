#include "TextureGrabber.h"

#include <mclib/protocol/PacketDispatcher.h>
#include <mclib/util/Hash.h>
#include <mclib/util/HTTPClient.h>

#include <iostream>
#include <fstream>


namespace example
{

TextureGrabber::TextureGrabber(PacketDispatcher* dispatcher)
	: PacketHandler(dispatcher)
{
	dispatcher->RegisterHandler(State::Play, play::PlayerListItem, this);
}

TextureGrabber::~TextureGrabber()
{
	GetDispatcher()->UnregisterHandler(this);
}

void TextureGrabber::HandlePacket(in::PlayerListItemPacket* packet)
{
	using namespace in;

	PlayerListItemPacket::Action action = packet->GetAction();

	if (action == PlayerListItemPacket::Action::AddPlayer)
	{
		auto actionDataList = packet->GetActionData();

		for (auto actionData : actionDataList)
		{
			auto properties = actionData->properties;

			auto iter = properties.find(L"textures");

			if (iter == properties.end()) continue;

			std::wstring encoded = iter->second;
			std::string decoded = Base64Decode(std::string(encoded.begin(), encoded.end()));
			std::wstring name = actionData->name;

			json root;
			try
			{
				root = json::parse(decoded);
			}
			catch (json::parse_error& e)
			{
				std::wcerr << e.what() << std::endl;
				continue;
			}

			if (!ContainsTextureURL(root))
			{
				std::wcerr << L"No texture found for " << name;
				continue;
			}

			std::string url = root["textures"]["SKIN"]["url"].get<std::string>();

			std::wcout << L"Fetching skin for " << name << std::endl;

			CurlHTTPClient http;

			HTTPResponse resp = http.Get(url);

			if (resp.status == 200)
			{
				std::wcout << L"Saving texture for " << name << std::endl;

				std::string body = resp.body;

				std::string filename = std::string(name.begin(), name.end()) + ".png";
				std::ofstream out(filename, std::ios::out | std::ios::binary);

				out.write(body.c_str(), body.size());
			}
		}
	}
}



//
// Private
//
bool TextureGrabber::ContainsTextureURL(const json& root)
{
	if (root.find("textures") == root.end())
		return false;

	if (root["textures"].find("SKIN") == root["textures"].end())
		return false;

	return root["textures"]["SKIN"].find("url") != root["textures"]["SKIN"].end();
}

} // ns example
