#include "stdafx.h"

#include <core/AuthToken.h>

#include <common/Json.h>
#include <util/Yggdrasil.h>

AuthToken::AuthToken()
	: m_AccessToken(""),
	m_ClientToken(""),
	m_ProfileId(""),
	m_Valid(false),
	m_Yggdrasil(std::make_unique<Yggdrasil>())
{}

AuthToken::AuthToken(const std::string& accessToken, const std::string& clientToken, const std::string& profileId)
	: m_AccessToken(accessToken),
	m_ClientToken(clientToken),
	m_ProfileId(profileId),
	m_Valid(false),
	m_Yggdrasil(std::make_unique<Yggdrasil>())
{}

AuthToken::AuthToken(const AuthToken& rhs)
	: m_AccessToken(rhs.m_AccessToken),
	m_ClientToken(rhs.m_ClientToken),
	m_ProfileId(rhs.m_ProfileId),
	m_Valid(rhs.m_Valid),
	m_Yggdrasil(std::make_unique<Yggdrasil>(*rhs.m_Yggdrasil))
{}

AuthToken& AuthToken::operator=(const AuthToken& rhs)
{
	m_AccessToken = rhs.m_AccessToken;
	m_ClientToken = rhs.m_ClientToken;
	m_ProfileId = rhs.m_ProfileId;
	m_Valid = rhs.m_Valid;
	m_Yggdrasil = std::make_unique<Yggdrasil>(*rhs.m_Yggdrasil);

	return *this;
}

bool AuthToken::Validate(const std::string& username)
{
	if (m_AccessToken.empty() || m_ClientToken.empty())
		return false;

	if (m_ProfileId.empty() && !username.empty())
	{
		auto http = std::make_unique<CurlHTTPClient>();
		auto result = http->Get("https://api.mojang.com/users/profiles/minecraft/" + username);
		if (result.status != 200)
			return false;

		json root;

		try
		{
			root = json::parse(result.body);
		}
		catch (json::parse_error&)
		{
			return false;
		}

		if (root.value("id", json()).is_string())
			m_ProfileId = root["id"].get<std::string>();
	}

	if (m_ProfileId.empty())
		return false;

	if (m_Yggdrasil->Validate(m_AccessToken, m_ClientToken))
	{
		m_Valid = true;
		m_Yggdrasil->SetProfileId(m_ProfileId);
		return true;
	}

	return false;
}

bool AuthToken::Refresh()
{
	try
	{
		auto pair = m_Yggdrasil->Refresh(m_AccessToken, m_ClientToken);
		m_AccessToken = pair.first;
		m_Valid = true;
	}
	catch (YggdrasilException&)
	{
		return false;
	}

	return true;
}
