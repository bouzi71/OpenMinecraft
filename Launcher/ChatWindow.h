#ifndef TERRACOTTA_CHAT_WINDOW_H_
#define TERRACOTTA_CHAT_WINDOW_H_

#include <mclib/protocol/PacketDispatcher.h>
#include <mclib/protocol/PacketHandler.h>


#include <deque>

namespace terra
{

	class ChatWindow : public PacketHandler
	{
	public:
		ChatWindow(PacketDispatcher* dispatcher, Connection* connection);
		~ChatWindow();

		void HandlePacket(in::ChatPacket* packet);
		void Render();

	private:
		Connection* m_Connection;
		char m_InputText[256];
		std::deque<std::string> m_ChatBuffer;
	};

} // ns terra

#endif
