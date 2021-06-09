#include "stdafx.h"

#include <protocol/PacketHandler.h>

#include <protocol/PacketDispatcher.h>


PacketHandler::PacketHandler(PacketDispatcher* dispatcher) 
	: m_Dispatcher(dispatcher) 
{}

PacketHandler::~PacketHandler()
{}

PacketDispatcher* PacketHandler::GetDispatcher()
{
	return m_Dispatcher;
}
