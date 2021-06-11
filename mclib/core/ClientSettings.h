#ifndef MCLIB_CORE_CLIENTSETTINGS_H
#define MCLIB_CORE_CLIENTSETTINGS_H


#include <common/Types.h>


class ClientSettings
{
private:
	std::wstring m_Locale;
	ChatMode m_ChatMode;
	MainHand m_MainHand;
	uint8 m_ViewDistance;
	uint8 m_SkinParts;
	bool m_ChatColors;

public:
	MCLIB_API ClientSettings();

	MCLIB_API ClientSettings(const ClientSettings& rhs) = default;
	MCLIB_API ClientSettings& operator=(const ClientSettings& rhs) = default;
	MCLIB_API ClientSettings(ClientSettings&& rhs) = default;
	MCLIB_API ClientSettings& operator=(ClientSettings&& rhs) = default;

	MCLIB_API ClientSettings& SetLocale(const std::wstring& locale);
	MCLIB_API ClientSettings& SetChatMode(ChatMode mode) noexcept;
	MCLIB_API ClientSettings& SetMainHand(MainHand main) noexcept;
	MCLIB_API ClientSettings& SetViewDistance(uint8 distance) noexcept;
	MCLIB_API ClientSettings& SetViewDistance(int32 distance) noexcept;
	MCLIB_API ClientSettings& SetSkinParts(uint8 parts) noexcept;
	MCLIB_API ClientSettings& SetChatColors(bool chatColors) noexcept;

	MCLIB_API ClientSettings& SetSkinPart(SkinPart part, bool display) noexcept;

	MCLIB_API inline const std::wstring& GetLocale() const noexcept { return m_Locale; }
	MCLIB_API inline ChatMode GetChatMode() const noexcept { return m_ChatMode; }
	MCLIB_API inline MainHand GetMainHand() const noexcept { return m_MainHand; }
	MCLIB_API inline uint8 GetViewDistance() const noexcept { return m_ViewDistance; }
	MCLIB_API inline uint8 GetSkinParts() const noexcept { return m_SkinParts; }
	MCLIB_API inline bool GetChatColors() const noexcept { return m_ChatColors; }
};

#endif
