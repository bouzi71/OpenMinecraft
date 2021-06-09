#include "stdafx.h"

#include <util/VersionFetcher.h>

#include <common/Json.h>


#include <iostream>

VersionFetcher::VersionFetcher(const std::string& host, u16 port)
	: m_Dispatcher(),
	m_Version(Version::Minecraft_1_12_2),
	m_Forge(&m_Dispatcher, nullptr),
	m_Connection(nullptr),
	m_Host(host),
	m_Port(port),
	m_Found(false)
{

}

void VersionFetcher::OnPingResponse(const json& node)
{
	static const std::map<s32, Version> mapping = {
		{ 340, Version::Minecraft_1_12_2 },
	};

	auto&& versionNode = node.value("version", json());

	if (versionNode.is_object())
	{
		auto&& protocolNode = versionNode.value("protocol", json());

		if (protocolNode.is_number_integer())
		{
			s32 protocol = protocolNode.get<int>();

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

	CMinecraftClient m_Client(&m_Dispatcher);

	m_Connection = m_Client.GetConnection();

	m_Client.GetConnection()->RegisterListener(this);
	m_Client.Ping(m_Host, m_Port, UpdateMethod::Block);
	m_Client.GetConnection()->UnregisterListener(this);

	return m_Version;
}
