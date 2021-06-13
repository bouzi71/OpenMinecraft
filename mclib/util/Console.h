#pragma once


class IMinecraftConsole
{
public:
	virtual void Output(const std::string& str) = 0;
	virtual void Output(const std::wstring& str) = 0;

	virtual IMinecraftConsole& operator<<(const std::string& str) = 0;
	virtual IMinecraftConsole& operator<<(const std::wstring& str) = 0;

	template <typename T>
	IMinecraftConsole& operator<<(T data)
	{
		Output(std::to_string(data));
		return *this;
	}

	IMinecraftConsole& operator<<(const char* data)
	{
		Output(std::string(data));
		return *this;
	}

	IMinecraftConsole& operator<<(const wchar_t* data)
	{
		Output(std::wstring(data));
		return *this;
	}
};


class Console
{
public:
	Console()
		: m_Impl(nullptr)
	{}

	// Doesn't take control of impl
	void SetImpl(IMinecraftConsole* impl)
	{
		m_Impl = impl;
	}

	IMinecraftConsole* GetImpl() const
	{
		return m_Impl;
	}

	void Output(const std::string& str)
	{
		if (m_Impl)
			m_Impl->Output(str);
	}

	void Output(const std::wstring& str)
	{
		if (m_Impl)
			m_Impl->Output(str);
	}

	template <typename T>
	Console& operator<<(const std::string& str)
	{
		Output(str);
		return *this;
	}

	template <typename T>
	Console& operator<<(const std::wstring& str)
	{
		Output(str);
		return *this;
	}

	template <typename T>
	Console& operator<<(const T& data)
	{
		Output(std::to_string(data));
		return *this;
	}

	Console& operator<<(const char* str)
	{
		Output(std::string(str));
		return *this;
	}

	Console& operator<<(const wchar_t* str)
	{
		Output(std::wstring(str));
		return *this;
	}

private:
	IMinecraftConsole* m_Impl;
};


class LoggerConsole
	: public IMinecraftConsole
{
public:
	LoggerConsole(const std::string& filename)
	{
		m_Out.open(filename.c_str(), std::ios::out);
	}

	~LoggerConsole()
	{
		m_Out.close();
	}

	void Output(const std::string& str) override
	{
		m_Out << str << std::endl;
	}

	void Output(const std::wstring& str) override
	{
		m_Out << std::string(str.begin(), str.end()) << std::endl;
	}

	IMinecraftConsole& operator<<(const std::string& str) override
	{
		Output(str);
		return *this;
	}
	IMinecraftConsole& operator<<(const std::wstring& str) override
	{
		Output(str);
		return *this;
	}

private:
	std::ofstream m_Out;
	std::string m_Filename;
};