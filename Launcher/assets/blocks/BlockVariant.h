#pragma once

#ifndef TERRACOTTA_BLOCK_BLOCKVARIANT_H_
#define TERRACOTTA_BLOCK_BLOCKVARIANT_H_

#include "block/Block.h"

#include "BlockModel.h"

class BlockVariant
{
public:
	BlockVariant(const CMinecraftBlock* block)
		: m_Block(block)
		, m_Model(nullptr)
		, m_Rotations(0.0f, 0.0f, 0.0f)
		, m_HasRotation(false) 
	{}


	const CMinecraftBlock* GetBlock() const noexcept
	{
		return m_Block;
	}


	const BlockModel* GetModel() const noexcept 
	{ 
		if (m_Model == nullptr)
		{
			_ASSERT(false == m_Subvariants.empty());
			return m_Subvariants[0].get()->GetModel();
		}

		return m_Model; 
	}
	void SetModel(const BlockModel* Model) noexcept
	{
		_ASSERT(m_Subvariants.empty());
		m_Model = Model;
	}


	const std::vector<std::unique_ptr<BlockVariant>>& GetSubvariants() const
	{
		return m_Subvariants;
	}

	void AddSubvariant(std::unique_ptr<BlockVariant> Subvariant)
	{
		m_Subvariants.push_back(std::move(Subvariant));
	}

	

	const glm::vec3& GetRotations() const noexcept
	{ 
		if (m_Model == nullptr)
		{
			_ASSERT(false == m_Subvariants.empty());
			return m_Subvariants[0].get()->GetRotations();
		}

		return m_Rotations; 
	}
	void SetRotation(const glm::vec3& rotations) noexcept
	{
		m_Rotations = rotations;
		m_HasRotation = (m_Rotations.x != 0.0f) || (m_Rotations.y != 0.0f) || (m_Rotations.z != 0.0f);
	}
	bool HasRotation() const noexcept
	{
		if (m_Model == nullptr)
		{
			_ASSERT(false == m_Subvariants.empty());
			return m_Subvariants[0].get()->HasRotation();
		}

		return m_HasRotation;
	}

private:
	const CMinecraftBlock* m_Block;
	const BlockModel* m_Model;
	glm::vec3 m_Rotations;
	bool m_HasRotation;
	std::vector<std::unique_ptr<BlockVariant>> m_Subvariants;
};

#endif
