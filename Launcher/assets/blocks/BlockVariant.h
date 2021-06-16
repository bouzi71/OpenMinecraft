#pragma once

#ifndef TERRACOTTA_BLOCK_BLOCKVARIANT_H_
#define TERRACOTTA_BLOCK_BLOCKVARIANT_H_

#include "block/Block.h"

#include "BlockModel.h"

class BlockVariant
{
public:
	BlockVariant(const BlockModel* model, const CMinecraftBlock* block) 
		: m_Model(model)
		, m_Block(block)
		, m_Rotations(0.0f, 0.0f, 0.0f)
		, m_HasRotation(false) 
	{}

	const BlockModel* GetModel() const noexcept 
	{ 
		return m_Model; 
	}

	const CMinecraftBlock* GetBlock() const noexcept 
	{ 
		return m_Block; 
	}

	const glm::vec3& GetRotations() const noexcept
	{ 
		return m_Rotations; 
	}

	void SetRotation(const glm::vec3& rotations) noexcept
	{
		m_Rotations = rotations;
		m_HasRotation = (m_Rotations.x != 0.0f) || (m_Rotations.y != 0.0f) || (m_Rotations.z != 0.0f);
	}

	bool HasRotation() const noexcept
	{
		return m_HasRotation;
	}

private:
	const BlockModel* m_Model;
	const CMinecraftBlock* m_Block;
	glm::vec3 m_Rotations;
	bool m_HasRotation;
};

#endif
