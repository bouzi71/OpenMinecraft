#ifndef MCLIB_COMMON_NAMEABLE_H_
#define MCLIB_COMMON_NAMEABLE_H_

class Nameable
{
public:
	virtual ~Nameable() {}

	const std::wstring& GetCustomName() const noexcept { return m_CustomName; }
	void SetCustomName(const std::wstring& name) { m_CustomName = name; }

private:
	std::wstring m_CustomName;
};

#endif
