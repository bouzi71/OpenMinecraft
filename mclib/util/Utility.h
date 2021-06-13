#ifndef MCLIB_UTIL_UTILITY_H
#define MCLIB_UTIL_UTILITY_H

#include <common/JsonFwd.h>
#include <common/Vector.h>
#include <core/AuthToken.h>
#include <core/Client.h>
#include <core/Connection.h>
#include <core/PlayerManager.h>
#include <world/World.h>

MCLIB_API bool GetProfileToken(const std::string& username, AuthToken* token);
MCLIB_API std::string ParseChatNode(const json& node);
MCLIB_API std::string StripChatMessage(const std::string& message);
MCLIB_API int64 GetTime();


#endif // UTILITY_H
