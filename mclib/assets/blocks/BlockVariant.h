#pragma once

#ifndef TERRACOTTA_BLOCK_BLOCKVARIANT_H_
#define TERRACOTTA_BLOCK_BLOCKVARIANT_H_

#include "BlockModel.h"

#include <block/Block.h>
#include <assets/blocks/BlockModel.h>
#include <glm/glm/glm.hpp>


class BlockVariant
{
public:
	BlockVariant(BlockModel* model, const CMinecraftBlock* block) 
		: m_Model(model)
		, m_Block(block)
		, m_Rotations(0.0f, 0.0f, 0.0f)
		, m_LockUV(true)
		, m_HasRotation(false) 
	{}

	BlockModel* GetModel() const { return m_Model; }
	const CMinecraftBlock* GetBlock() const { return m_Block; }

	const glm::vec3& GetRotations() const 
	{ 
		return m_Rotations; 
	}
	void SetRotation(const glm::vec3& rotations)
	{
		m_Rotations = rotations;
		m_HasRotation = (m_Rotations.x != 0.0f) || (m_Rotations.y != 0.0f) || (m_Rotations.z != 0.0f);
	}

	bool HasRotation() const
	{
		return m_HasRotation;
	}

private:
	BlockModel* m_Model;
	const CMinecraftBlock* m_Block;
	glm::vec3 m_Rotations;
	bool m_LockUV;
	bool m_HasRotation;
};

#endif
