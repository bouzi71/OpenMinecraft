#ifndef MCLIB_BLOCK_BLOCK_H_
#define MCLIB_BLOCK_BLOCK_H_

#include <common/AABB.h>
#include <common/Types.h>
#include <protocol/ProtocolState.h>

class CMinecraftBlock
{
	friend class BlockRegistry;
public:
	explicit CMinecraftBlock(const std::string& name, uint32 ID, bool IsTransperent, bool IsSolid)
		: m_Name(name)
		, m_ID(ID)
		, m_IsTransperent(IsTransperent)
		, m_Solid(IsSolid)
	{
		//_ASSERT(false == name.empty());
	}

	explicit CMinecraftBlock(const std::string& name, uint32 Type, uint32 Meta, bool IsTransperent, bool IsSolid)
		: m_Name(name)
		, m_ID(Type << 4 | (Meta & 15))
		, m_IsTransperent(IsTransperent)
		, m_Solid(IsSolid)
	{
		//_ASSERT(false == name.empty());
	}

	explicit CMinecraftBlock(const std::string& name, uint32 ID, bool IsTransperent, bool IsSolid, const CMinecraftAABB& bounds)
		: m_Name(name)
		, m_ID(ID)
		, m_IsTransperent(IsTransperent)
		, m_Solid(IsSolid)
		, m_BoundingBox(bounds)
	{
		//_ASSERT(false == name.empty());
	}

	virtual ~CMinecraftBlock() 
	{}

	CMinecraftBlock(const CMinecraftBlock& other) = delete;
	CMinecraftBlock& operator=(const CMinecraftBlock& rhs) = delete;
	CMinecraftBlock(CMinecraftBlock&& other) = delete;
	CMinecraftBlock& operator=(CMinecraftBlock&& rhs) = delete;

	bool operator==(const CMinecraftBlock& other) noexcept
	{
		return m_ID == other.m_ID;
	}

	virtual std::string GetName() const 
	{ 
		return m_Name; 
	}

	uint32 GetID() const noexcept
	{
		return m_ID;
	}

	uint32 GetType() const noexcept
	{
		return (GetID() >> 4);
	}

	uint32 GetMeta() const noexcept
	{
		return (GetID() & 15);
	}

	bool IsTransperent() const noexcept
	{
		return m_IsTransperent;
	}

	bool IsSolid() const noexcept
	{
		return m_Solid;
	}

	bool IsOpaque() const noexcept
	{
		return m_BoundingBox.min != glm::dvec3(0, 0, 0) || m_BoundingBox.max != glm::dvec3(0, 0, 0);
	}

	virtual CMinecraftAABB GetBoundingBox() const noexcept
	{
		return m_BoundingBox;
	}

	virtual CMinecraftAABB GetBoundingBox(glm::ivec3 at) const // at is the world position of this block. Used to get the world bounding box
	{
		glm::dvec3 atd = at;
		CMinecraftAABB bounds = m_BoundingBox;
		bounds.min += atd;
		bounds.max += atd;
		return bounds;
	}

	virtual CMinecraftAABB GetBoundingBox(glm::dvec3 at) const
	{
		return GetBoundingBox(at);
	}

	virtual std::pair<bool, CMinecraftAABB> CollidesWith(glm::ivec3 at, const CMinecraftAABB& other) const
	{
		CMinecraftAABB boundingBox = GetBoundingBox() + at;
		return std::make_pair(boundingBox.Intersects(other), boundingBox);
	}

	virtual std::vector<CMinecraftAABB> GetBoundingBoxes() const // Returns the raw unmodified-by-position bounding boxes.
	{
		return std::vector<CMinecraftAABB>(1, m_BoundingBox);
	}

	void SetBoundingBox(const CMinecraftAABB& bound) noexcept
	{
		m_BoundingBox = bound;
	}


	//
	// Variables
	//
	const std::vector<std::string>& GetVariables() const noexcept
	{
		return m_Variables;
	}

	void AddVariable(const std::string& Variable) noexcept
	{
		m_Variables.push_back(Variable);
	}

	bool IsVariablesMatch(const std::vector<std::string>& OtherVariables) const noexcept
	{
		if (OtherVariables.empty() && m_Variables.empty())
			return true;

		for (const auto& otherVariableIt : OtherVariables)
		{
			// hack
			//if (m_Variables.size() > 1 && otherVariableIt.find("shape=") != std::string::npos) // shapes don't match
			//	continue;

			if (std::find(m_Variables.begin(), m_Variables.end(), otherVariableIt) == m_Variables.end())
				return false;
		}
		return true;
	}



	//
	// Blockmetas
	//
	const std::map<uint32, const CMinecraftBlock*>& GetBlockmetas() const noexcept
	{
		return m_Blockmetas;
	}

	const CMinecraftBlock* GetBlockmeta(uint32 Meta) const noexcept
	{
		const auto& blockmetasIt = m_Blockmetas.find(Meta);
		if (blockmetasIt == m_Blockmetas.end())
			return nullptr;
		return (blockmetasIt->second);
	}

	void AddBlockmeta(const CMinecraftBlock* Blockmeta) noexcept
	{
		m_Blockmetas[Blockmeta->GetMeta()] = Blockmeta;
	}

protected:
	std::string m_Name;
	uint32 m_ID;
	bool m_IsTransperent;
	bool m_Solid;
	CMinecraftAABB m_BoundingBox;
	std::vector<std::string> m_Variables;
	std::map<uint32, const CMinecraftBlock*> m_Blockmetas;
};


#endif
