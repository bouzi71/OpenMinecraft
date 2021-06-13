#include "stdafx.h"

// General
#include "Console.h"

static Console console;

class StandardConsole 
	: public IMinecraftConsole
{
public:
	void Output(const std::string& str)
	{
		Log::Print(str.c_str());
	}

	void Output(const std::wstring& str)
	{
		Log::Print(Resources::utf16_to_utf8(str).c_str());
	}

	IMinecraftConsole& operator<<(const std::string& str)
	{
		Output(str);
		return *this;
	}
	IMinecraftConsole& operator<<(const std::wstring& str)
	{
		Output(str);
		return *this;
	}
};