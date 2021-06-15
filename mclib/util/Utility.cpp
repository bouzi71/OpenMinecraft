#include "stdafx.h"

// General
#include "Utility.h"

#include <block/Block.h>
#include <common/DataBuffer.h>
#include <common/Json.h>
#include <core/Connection.h>
#include <core/PlayerManager.h>
#include <entity/EntityManager.h>
#include <protocol/Protocol.h>
#include <world/World.h>

#undef max
#undef min

#ifdef _WIN32
#include <ShlObj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif


#ifdef _WIN32

std::wstring GetAppDataPath()
{
	WCHAR* appdata = nullptr;
	if (::SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appdata) != S_OK)
		return false;

	std::wstring result(appdata);

	::CoTaskMemFree(appdata);

	return result;
}

#else

std::wstring GetAppDataPath()
{
	const char* home = nullptr;

	if ((home = getenv("HOME")) == nullptr)
		home = getpwuid(getuid())->pw_dir;

	return to_wstring(home);
}

#endif

bool GetProfileToken(const std::string& username, AuthToken* token)
{
	std::wstring appData = GetAppDataPath();

	const std::wstring filename = appData + L"/.minecraft/launcher_profiles.json";

	std::ifstream in(std::string(filename.begin(), filename.end()));

	if (!in.is_open())
		return false;

	json root;

	try
	{
		root = json::parse(in);
	}
	catch (json::parse_error& e)
	{
		Log::Error("GetProfileToken: Parse error '%s'.", e.what());
		return false;
	}

	if (root.value("clientToken", json()).is_null() || root.value("authenticationDatabase", json()).is_null())
		return false;

	auto& authDB = root["authenticationDatabase"];

	std::string accessToken;
	std::string clientToken;

	clientToken = root["clientToken"].get<std::string>();

	for (auto& member : authDB.items())
	{
		auto& account = member.value();

		auto accessTokenNode = account.value("accessToken", json());
		if (accessTokenNode.is_null())
			continue;

		accessToken = accessTokenNode.get<std::string>();

		auto profilesNode = account.value("profiles", json());
		// Check if it's using Linux format
		if (profilesNode.is_null())
		{
			if (account.find("displayName") == account.end())
				continue;

			if (account.find("uuid") == account.end())
				continue;

			if (account["displayName"] != username)
				continue;

			*token = AuthToken(accessToken, clientToken, member.key());
			return true;
		}
		else
		{
			for (auto& kv : profilesNode.items())
			{
				std::string profileId = kv.key();
				auto& profile = kv.value();

				auto nameNode = profile.value("displayName", json());
				if (nameNode.is_null())
					continue;

				if (nameNode.get<std::string>() != username)
					continue;

				*token = AuthToken(accessToken, clientToken, profileId);
				return true;
			}
		}
	}

	return false;
}

int64 GetTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Slot CreateFirework(bool flicker, bool trail, uint8 type, uint8 duration, std::vector<int> colors, const std::string& name = "")
{
	NBT nbt;

	TagCompound* fireworks = new TagCompound(L"Fireworks");
	TagPtr flightTag(new TagByte("Flight", duration));

	fireworks->AddItem(TagType::Byte, flightTag);

	TagList* explosions = new TagList("Explosions", TagType::Compound);

	TagCompound* explosion = new TagCompound(L"Explosion");
	TagPtr flickerTag(new TagByte("Flicker", flicker ? 1 : 0));
	TagPtr trailTag(new TagByte("Trail", trail ? 1 : 0));
	TagPtr typeTag(new TagByte("Type", type));

	TagPtr colorsTag(new TagIntArray("Colors", colors));
	//TagPtr fadeColorsTag(new TagIntArray("FadeColors", colors));

	explosion->AddItem(TagType::Byte, flickerTag);
	explosion->AddItem(TagType::Byte, trailTag);
	explosion->AddItem(TagType::Byte, typeTag);
	explosion->AddItem(TagType::IntArray, colorsTag);

	explosions->AddItem(TagPtr(explosion));
	fireworks->AddItem(TagType::Compound, TagPtr(explosions));
	nbt.GetRoot().AddItem(TagType::Compound, TagPtr(fireworks));
	nbt.GetRoot().SetName(L"tag");

	if (!name.empty())
	{
		TagCompound* display = new TagCompound(L"display");
		TagPtr nameTag(new TagString("Name", name));

		display->AddItem(TagType::String, nameTag);

		nbt.GetRoot().AddItem(TagType::Compound, TagPtr(display));
	}

	Slot slot(401, 64, 0, nbt);

	return slot;
}

std::string ParseChatNode(const json& node)
{
	if (node.is_object())
	{
		auto translateNode = node.value("translate", json());

		if (translateNode.is_string())
		{
			std::string translate = translateNode.get<std::string>();
			std::string message;

			if (translate == "chat.type.text")
			{
				auto withNode = node.value("with", json());

				if (withNode.is_array())
				{
					for (auto iter = withNode.begin(); iter != withNode.end(); ++iter)
					{
						auto&& node = *iter;

						if (node.is_object())
						{
							auto&& textNode = node.value("text", json());
							if (textNode.is_string())
								message += "<" + textNode.get<std::string>() + "> ";
						}
						else if (node.is_string())
						{
							message += node.get<std::string>();
						}
					}
				}
			}

			return message;
		}
	}

	if (node.is_null()) return "";
	if (node.is_string()) return node.get<std::string>();
	if (node.is_object())
	{
		std::string result;

		auto extraNode = node.value("extra", json());
		auto textNode = node.value("text", json());

		if (!extraNode.is_null())
			result += ParseChatNode(extraNode);
		if (textNode.is_string())
			result += textNode.get<std::string>();
		return result;
	}
	if (node.is_array())
	{
		std::string result;

		for (auto arrayNode : node)
			result += ParseChatNode(arrayNode);
		return result;
	}
	return "";
}






std::string StripChatMessage(const std::string& message)
{
	std::wstring utf16 = utf8to16(message);

	std::size_t pos = utf16.find((wchar_t)0xA7);

	while (pos != std::string::npos)
	{
		utf16.erase(pos, 2);
		pos = utf16.find((wchar_t)0xA7);
	}

	return utf16to8(utf16);
}
