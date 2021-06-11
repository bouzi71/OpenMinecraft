#include "stdafx.h"

#include <core/ClientSettings.h>

constexpr auto FullSkin = (uint8)((uint8)SkinPart::Cape | (uint8)SkinPart::Jacket | (uint8)SkinPart::LeftSleeve | (uint8)SkinPart::RightSleeve | (uint8)SkinPart::LeftPants | (uint8)SkinPart::RightPants | (uint8)SkinPart::Hat);

ClientSettings::ClientSettings()
	: m_Locale(L"en_GB"),
	m_ChatMode(ChatMode::Enabled),
	m_MainHand(MainHand::Right),
	m_ViewDistance(16),
	m_SkinParts(FullSkin),
	m_ChatColors(true)
{

}

ClientSettings& ClientSettings::SetLocale(const std::wstring& locale)
{
	m_Locale = locale;
	return *this;
}

ClientSettings& ClientSettings::SetChatMode(ChatMode mode) noexcept
{
	m_ChatMode = mode;
	return *this;
}

ClientSettings& ClientSettings::SetMainHand(MainHand main) noexcept
{
	m_MainHand = main;
	return *this;
}

ClientSettings& ClientSettings::SetViewDistance(uint8 distance) noexcept
{
	m_ViewDistance = distance;
	return *this;
}

ClientSettings& ClientSettings::SetViewDistance(int32 distance) noexcept
{
	this->SetViewDistance((uint8)distance);
	return *this;
}

ClientSettings& ClientSettings::SetSkinParts(uint8 parts) noexcept
{
	m_SkinParts = parts;
	return *this;
}
ClientSettings& ClientSettings::SetChatColors(bool chatColors) noexcept
{
	m_ChatColors = chatColors;
	return *this;
}

ClientSettings& ClientSettings::SetSkinPart(SkinPart part, bool display) noexcept
{
	if (display)
	{
		m_SkinParts |= (uint8)part;
	}
	else
	{
		m_SkinParts &= ~(uint8)part;
	}
	return *this;
}
