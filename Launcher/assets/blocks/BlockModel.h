#ifndef TERRACOTTA_BLOCK_BLOCKMODEL_H_
#define TERRACOTTA_BLOCK_BLOCKMODEL_H_

#include <common/JsonFwd.h>

#include "BlockFace.h"
#include "BlockElement.h"

class BlockModel
{
public:
	const std::vector<BlockElement>& GetElements() const noexcept { return m_Elements; }
	void AddElement(BlockElement element) { m_Elements.push_back(element); }

private:
	std::vector<BlockElement> m_Elements;
};

#endif
