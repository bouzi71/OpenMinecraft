#include "stdafx.h"

#include <protocol/PacketDispatcher.h>

#include <protocol/PacketHandler.h>

#include <algorithm>


int32 GetDispatcherId(CMinecraftPacket* packet)
{
	auto version = packet->GetProtocolVersion();

	Protocol protocol = Protocol::GetProtocol(version);

	int32 agnosticId = 0;
	if (!protocol.GetAgnosticId(packet->GetProtocolState(), packet->GetId().GetInt(), agnosticId))
	{
		throw std::runtime_error(std::string("Unknown packet type ") + std::to_string(packet->GetId().GetInt()) + " received");
	}

	return agnosticId;
}

void PacketDispatcher::RegisterHandler(State protocolState, PacketId id, PacketHandler* handler)
{
	PacketType type(protocolState, id);
	std::vector<PacketHandler*>::iterator found = std::find(m_Handlers[type].begin(), m_Handlers[type].end(), handler);
	if (found == m_Handlers[type].end())
		m_Handlers[type].push_back(handler);
}

void PacketDispatcher::UnregisterHandler(State protocolState, PacketId id, PacketHandler* handler)
{
	PacketType type(protocolState, id);
	std::vector<PacketHandler*>::iterator found = std::find(m_Handlers[type].begin(), m_Handlers[type].end(), handler);
	if (found != m_Handlers[type].end())
		m_Handlers[type].erase(found);
}

void PacketDispatcher::UnregisterHandler(PacketHandler* handler)
{
	for (auto& pair : m_Handlers)
	{
		if (pair.second.empty()) continue;

		auto state = pair.first.first;
		auto id = pair.first.second;
		PacketType type(state, id);

		m_Handlers[type].erase(std::remove(m_Handlers[type].begin(), m_Handlers[type].end(), handler), m_Handlers[type].end());
	}
}

void PacketDispatcher::Dispatch(CMinecraftPacket* packet)
{
	if (!packet) return;

	auto state = packet->GetProtocolState();
	int64 id = GetDispatcherId(packet);

	if (packet->GetProtocolState() != State::Play)
		printf("Received: Id '0x%X'. State '%d'.\r\n", packet->GetId().GetInt(), packet->GetProtocolState());

	PacketType type(state, id);
	for (PacketHandler* handler : m_Handlers[type])
		packet->Dispatch(handler);
}
