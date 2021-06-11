#ifndef TERRACOTTA_BLOCK_BLOCKSTATE_H_
#define TERRACOTTA_BLOCK_BLOCKSTATE_H_

#include <common/Types.h>
#include <common/JsonFwd.h>

class BlockState
{
public:
	BlockState(uint32 id) : m_Id(id) {}

	uint32 GetId() const { return m_Id; }
	const std::string& GetProperty(const std::string& property) const;
	const std::string& GetVariant() const { return m_Variant; }

	static std::unique_ptr<BlockState> FromJSON(const json& json);

private:
	uint32 m_Id;
	std::string m_Variant;
	std::unordered_map<std::string, std::string> m_Properties;
};

#endif
