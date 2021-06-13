#include "stdafx.h"

#include "ChatWindow.h"

#include <util/Utility.h>
#include <cstring>


namespace
{

	constexpr std::size_t kChatBufferLines = 50;

} // ns

namespace terra
{

	ChatWindow::ChatWindow(PacketDispatcher* dispatcher, Connection* connection)
		: PacketHandler(dispatcher)
		, m_Connection(connection)
	{
		//memset(m_InputText, 0, IM_ARRAYSIZE(m_InputText));

		dispatcher->RegisterHandler(State::Play, play::Chat, this);
	}

	ChatWindow::~ChatWindow()
	{
		GetDispatcher()->UnregisterHandler(this);
	}

	void ChatWindow::Render()
	{
		/*ImGui::SetNextWindowBgAlpha(0.3f);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("chat", 0, flags);

		for (auto&& str : m_ChatBuffer)
		{
			ImGui::TextWrapped("%s", str.c_str());
		}

		ImGui::SetScrollHereY(1.0f);

		ImGui::End();

		int width = static_cast<int>(ImGui::GetWindowContentRegionWidth());
		flags = ImGuiWindowFlags_NoDecoration;
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowSize(ImVec2(400, ImGui::GetTextLineHeightWithSpacing() + 5));
		ImGui::Begin("chat_input", 0, flags);

		ImGui::PushItemWidth(400);
		if (ImGui::InputText("", m_InputText, IM_ARRAYSIZE(m_InputText), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			out::ChatPacket packet(m_InputText);

			m_Connection->SendPacket(&packet);

			memset(m_InputText, 0, IM_ARRAYSIZE(m_InputText));
		}
		ImGui::PopItemWidth();
		ImGui::End();*/
	}

	void ChatWindow::HandlePacket(in::ChatPacket* packet)
	{
		auto&& data = packet->GetChatData();
		std::string str = ParseChatNode(data);
		str = StripChatMessage(str);

		m_ChatBuffer.push_back(str);

		if (m_ChatBuffer.size() > kChatBufferLines)
		{
			m_ChatBuffer.pop_front();
		}
	}

} // ns terra
