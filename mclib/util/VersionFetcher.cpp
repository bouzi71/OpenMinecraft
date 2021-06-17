#include "stdafx.h"

#include <util/VersionFetcher.h>

#include <common/Json.h>


#include <iostream>

VersionFetcher::VersionFetcher(const std::string& host, uint16 port)
	: m_Dispatcher(),
	m_Version(Version::Minecraft_1_12_2),
	m_Connection(nullptr),
	m_Host(host),
	m_Port(port),
	m_Found(false)
{

}

void VersionFetcher::OnPingResponse(const json& node)
{
	Log::Print("VersionFetcher::OnPingResponse.");

	static const std::map<int32, Version> mapping = {
		{ 340, Version::Minecraft_1_12_2 },
	};

	auto&& versionNode = node.value("version", json());

	if (versionNode.is_object())
	{
		auto&& protocolNode = versionNode.value("protocol", json());

		if (protocolNode.is_number_integer())
		{
			int32 protocol = protocolNode.get<int>();

			auto iter = mapping.lower_bound(protocol);
			if (iter != mapping.end())
			{
				m_Version = iter->second;
				m_Found = true;
				m_Connection->Disconnect();
			}
		}
	}

	m_Connection->Disconnect();
}

Version VersionFetcher::GetVersion()
{
	if (m_Found) 
		return m_Version;

	CMinecraftClient client(&m_Dispatcher);

	m_Connection = client.GetConnection();

	client.GetConnection()->RegisterListener(this);
	client.Ping(m_Host, m_Port, UpdateMethod::CMinecraftBlock);
	client.GetConnection()->UnregisterListener(this);

	return m_Version;
}
