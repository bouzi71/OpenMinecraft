#ifndef CLIENT_LOGGER_H_
#define CLIENT_LOGGER_H_

#include <mclib/core/Client.h>
#include <mclib/protocol/PacketHandler.h>

namespace example
{

class Logger 
	: public PacketHandler
	, public ClientListener
{
public:
	Logger(CMinecraftClient* client, PacketDispatcher* dispatcher);
	~Logger();

	void HandlePacket(in::ChatPacket* packet) override;
	void HandlePacket(in::EntityLookAndRelativeMovePacket* packet) override;
	void HandlePacket(in::BlockChangePacket* packet) override;
	void HandlePacket(in::MultiBlockChangePacket* packet) override;
	void HandlePacket(login::in::DisconnectPacket* packet) override;

	void OnTick() override;

private:
	CMinecraftClient* m_Client;
};

} // ns example

#endif
