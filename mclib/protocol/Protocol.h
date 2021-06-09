#ifndef MCLIB_PROTOCOL_PROTOCOL_H_
#define MCLIB_PROTOCOL_PROTOCOL_H_

#include <protocol/ProtocolState.h>
#include <protocol/packets/PacketsLogin.h>
#include <protocol/packets/PacketsStatus.h>
#include <protocol/packets/PacketsPlayOutbound.h>




using PacketCreator = CMinecraftInboundPacket* (*)();
using PacketMap = std::unordered_map<s32, s32>;

class UnsupportedPacketException 
	: public std::exception
{
private:
	std::string m_ErrorMessage;

public:
	UnsupportedPacketException(const std::string& message)
		: m_ErrorMessage(message)
	{}

	virtual const char* what() const noexcept
	{
		return m_ErrorMessage.c_str();
	}
};


class Protocol
{
public:
	typedef std::unordered_map<State, PacketMap> StateMap;

protected:
	StateMap m_InboundMap;
	Version m_Version;

public:
	Protocol(Version version, StateMap inbound)
		: m_InboundMap(inbound)
		, m_Version(version)
	{}
	virtual ~Protocol() 
	{}

	virtual Version GetVersion() const noexcept { return m_Version; }

	// Creates an inbound packet from state and packet id
	virtual CMinecraftInboundPacket* CreateInboundPacket(State state, s32 id);

	// Convert the protocol id into a protocol agnostic id. This is used as the dispatching id.
	bool GetAgnosticId(State state, s32 protocolId, s32& agnosticId);

	// Handshake
	virtual s32 GetPacketId(login::out::HandshakePacket) { return 0x00; }

	// Login
	virtual s32 GetPacketId(login::out::LoginStartPacket) { return 0x00; }
	virtual s32 GetPacketId(login::out::EncryptionResponsePacket) { return 0x01; }

	// Status
	virtual s32 GetPacketId(status::out::RequestPacket) { return 0x00; }
	virtual s32 GetPacketId(status::out::PingPacket) { return 0x01; }

	// Play
	virtual s32 GetPacketId(out::TeleportConfirmPacket) { return 0x00; }
	virtual s32 GetPacketId(out::TabCompletePacket) { return 0x01; }
	virtual s32 GetPacketId(out::ChatPacket) { return 0x02; }
	virtual s32 GetPacketId(out::ClientStatusPacket) { return 0x03; }
	virtual s32 GetPacketId(out::ClientSettingsPacket) { return 0x04; }
	virtual s32 GetPacketId(out::ConfirmTransactionPacket) { return 0x05; }
	virtual s32 GetPacketId(out::EnchantItemPacket) { return 0x06; }
	virtual s32 GetPacketId(out::ClickWindowPacket) { return 0x07; }
	virtual s32 GetPacketId(out::CloseWindowPacket) { return 0x08; }
	virtual s32 GetPacketId(out::PluginMessagePacket) { return 0x09; }
	virtual s32 GetPacketId(out::UseEntityPacket) { return 0x0A; }
	virtual s32 GetPacketId(out::KeepAlivePacket) { return 0x0B; }
	virtual s32 GetPacketId(out::PlayerPacket) { return 0x0C; }
	virtual s32 GetPacketId(out::PlayerPositionPacket) { return 0x0D; }
	virtual s32 GetPacketId(out::PlayerPositionAndLookPacket) { return 0x0E; }
	virtual s32 GetPacketId(out::PlayerLookPacket) { return 0x0F; }
	virtual s32 GetPacketId(out::VehicleMovePacket) { return 0x10; }
	virtual s32 GetPacketId(out::SteerBoatPacket) { return 0x11; }
	virtual s32 GetPacketId(out::CraftRecipeRequestPacket) { return 0x12; }
	virtual s32 GetPacketId(out::PlayerAbilitiesPacket) { return 0x13; }
	virtual s32 GetPacketId(out::PlayerDiggingPacket) { return 0x14; }
	virtual s32 GetPacketId(out::EntityActionPacket) { return 0x15; }
	virtual s32 GetPacketId(out::SteerVehiclePacket) { return 0x16; }
	virtual s32 GetPacketId(out::CraftingBookDataPacket) { return 0x17; }
	virtual s32 GetPacketId(out::ResourcePackStatusPacket) { return 0x18; }
	virtual s32 GetPacketId(out::AdvancementTabPacket) { return 0x19; }
	virtual s32 GetPacketId(out::HeldItemChangePacket) { return 0x1A; }
	virtual s32 GetPacketId(out::CreativeInventoryActionPacket) { return 0x1B; }
	virtual s32 GetPacketId(out::UpdateSignPacket) { return 0x1C; }
	virtual s32 GetPacketId(out::AnimationPacket) { return 0x1D; }
	virtual s32 GetPacketId(out::SpectatePacket) { return 0x1E; }
	virtual s32 GetPacketId(out::PlayerBlockPlacementPacket) { return 0x1F; }
	virtual s32 GetPacketId(out::UseItemPacket) { return 0x20; }

	virtual s32 GetPacketId(out::PrepareCraftingGridPacket) { throw UnsupportedPacketException("PrepareCraftingGridPacket does not work with protocol 1.12.1"); }

	static Protocol& GetProtocol(Version version);
};

#endif
