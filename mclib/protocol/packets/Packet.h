#ifndef MCLIB_PROTOCOL_PACKETS_PACKET_H_
#define MCLIB_PROTOCOL_PACKETS_PACKET_H_

#include <block/BlockEntity.h>
#include <common/DataBuffer.h>
#include <common/Json.h>
#include <common/MCString.h>
#include <common/Position.h>
#include <inventory/Slot.h>
#include <entity/Attribute.h>
#include <entity/Metadata.h>
#include <protocol/ProtocolState.h>
#include <world/Chunk.h>





template <typename T>
class FixedPointNumber
{
public:
	FixedPointNumber() : m_IntRep(0) {}
	FixedPointNumber(T intRep) : m_IntRep(intRep) {}

	float GetFloat() const
	{
		return (float)(m_IntRep >> 5) + (m_IntRep & 31) / 32.0f;
	}

	friend DataBuffer& operator>>(DataBuffer& in, FixedPointNumber<int8>& fpn);
	friend DataBuffer& operator>>(DataBuffer& in, FixedPointNumber<int32>& fpn);

private:
	T m_IntRep;
};

inline DataBuffer& operator>>(DataBuffer& in, FixedPointNumber<int8>& fpn)
{
	return in >> fpn.m_IntRep;
}

inline DataBuffer& operator>>(DataBuffer& in, FixedPointNumber<int32>& fpn)
{
	return in >> fpn.m_IntRep;
}


/**
 * Thanks to #mcdevs on irc.freenode.net (http://wiki.vg/Protocol) for protocol information.
 */

class Connection;
class PacketHandler;


class CMinecraftPacket
{
public:
	CMinecraftPacket() noexcept
		: m_Id(0xFF)
		, m_ProtocolState(State::Play)
		, m_Connection(nullptr)
		, m_ProtocolVersion(Version::Minecraft_1_12_2)
	{}
	virtual ~CMinecraftPacket() {}

	CMinecraftPacket(const CMinecraftPacket& rhs) = default;
	CMinecraftPacket& operator=(const CMinecraftPacket& rhs) = default;
	CMinecraftPacket(CMinecraftPacket&& rhs) = default;
	CMinecraftPacket& operator=(CMinecraftPacket&& rhs) = default;

	Version GetProtocolVersion() const noexcept { return m_ProtocolVersion; }
	void SetProtocolVersion(Version version) noexcept { m_ProtocolVersion = version; }

	CVarInt GetId() const noexcept { return m_Id; }
	void SetId(int32 id) { m_Id = id; }

	State GetProtocolState() const noexcept { return m_ProtocolState; }

	virtual DataBuffer Serialize() const = 0;
	virtual bool Deserialize(DataBuffer& data, std::size_t packetLength) = 0;

	virtual void Dispatch(PacketHandler* handler) = 0;

	MCLIB_API void SetConnection(Connection* connection) { m_Connection = connection; }
	MCLIB_API Connection* GetConnection() { return m_Connection; }

protected:
	CVarInt m_Id;
	State m_ProtocolState;
	Version m_ProtocolVersion;
	Connection* m_Connection;
};


class CMinecraftInboundPacket 
	: public CMinecraftPacket
{
public:
	virtual ~CMinecraftInboundPacket() 
	{}

	DataBuffer Serialize() const 
	{ 
		return DataBuffer(); 
	}
};


class CMinecraftOutboundPacket 
	: public CMinecraftPacket
{
public:
	virtual ~CMinecraftOutboundPacket() {}

	bool Deserialize(DataBuffer& data, std::size_t packetLength) { return false; }
	void Dispatch(PacketHandler* handler)
	{
		throw std::runtime_error("Cannot dispatch an outbound packet.");
	}
};


#endif
