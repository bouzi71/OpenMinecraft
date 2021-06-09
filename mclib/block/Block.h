#ifndef MCLIB_BLOCK_BLOCK_H_
#define MCLIB_BLOCK_BLOCK_H_

#include <common/AABB.h>
#include <common/Types.h>
#include <protocol/ProtocolState.h>

class Block
{
	friend class BlockRegistry;
public:
	explicit Block(const std::string& name, u32 Type, u32 Meta, bool IsTransperent = false, bool IsSolid = true)
		: m_Name(name)
		, m_ID(Type << 4 | (Meta & 15))
		, m_IsTransperent(IsTransperent)
		, m_Solid(IsSolid)
	{
		//_ASSERT(false == name.empty());
	}

	explicit Block(const std::string& name, u32 ID, bool IsTransperent, bool IsSolid, const AABB& bounds)
		: m_Name(name)
		, m_ID(ID)
		, m_IsTransperent(IsTransperent)
		, m_Solid(IsSolid)
		, m_BoundingBox(bounds)
	{
		//_ASSERT(false == name.empty());
	}

	virtual ~Block() 
	{}

	Block(const Block& other) = delete;
	Block& operator=(const Block& rhs) = delete;
	Block(Block&& other) = delete;
	Block& operator=(Block&& rhs) = delete;

	bool operator==(const Block& other) noexcept
	{
		return m_ID == other.m_ID;
	}

	virtual std::string GetName() const 
	{ 
		return m_Name; 
	}

	u32 GetID() const noexcept
	{
		return m_ID;
	}

	u32 GetType() const noexcept
	{
		return (GetID() >> 4);
	}

	u32 GetMeta() const noexcept
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
		return m_BoundingBox.min != Vector3d(0, 0, 0) || m_BoundingBox.max != Vector3d(0, 0, 0);
	}

	virtual AABB GetBoundingBox() const noexcept
	{
		return m_BoundingBox;
	}

	virtual AABB GetBoundingBox(Vector3i at) const // at is the world position of this block. Used to get the world bounding box
	{
		Vector3d atd = ToVector3d(at);
		AABB bounds = m_BoundingBox;
		bounds.min += atd;
		bounds.max += atd;
		return bounds;
	}

	virtual AABB GetBoundingBox(Vector3d at) const
	{
		return GetBoundingBox(ToVector3i(at));
	}

	virtual std::pair<bool, AABB> CollidesWith(Vector3i at, const AABB& other) const
	{
		AABB boundingBox = GetBoundingBox() + at;
		return std::make_pair(boundingBox.Intersects(other), boundingBox);
	}

	virtual std::vector<AABB> GetBoundingBoxes() const // Returns the raw unmodified-by-position bounding boxes.
	{
		return std::vector<AABB>(1, m_BoundingBox);
	}

	void SetBoundingBox(const AABB& bound) noexcept
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
	const std::map<u32, const Block*>& GetBlockmetas() const noexcept
	{
		return m_Blockmetas;
	}

	const Block* GetBlockmeta(u32 Meta) const noexcept
	{
		const auto& blockmetasIt = m_Blockmetas.find(Meta);
		if (blockmetasIt == m_Blockmetas.end())
			return nullptr;
		return (blockmetasIt->second);
	}

	void AddBlockmeta(const Block* Blockmeta) noexcept
	{
		m_Blockmetas[Blockmeta->GetMeta()] = Blockmeta;
	}

protected:
	std::string m_Name;
	u32 m_ID;
	bool m_IsTransperent;
	bool m_Solid;
	AABB m_BoundingBox;
	std::vector<std::string> m_Variables;
	std::map<u32, const Block*> m_Blockmetas;
};


#endif
