#ifndef MCLIB_UTIL_UTILITY_H
#define MCLIB_UTIL_UTILITY_H

#include <common/JsonFwd.h>
#include <core/AuthToken.h>

MCLIB_API bool GetProfileToken(const std::string& username, AuthToken* token);
MCLIB_API std::string ParseChatNode(const json& node);
MCLIB_API std::string StripChatMessage(const std::string& message);
MCLIB_API int64 GetTime();


#endif // UTILITY_H
