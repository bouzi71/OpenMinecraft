#ifndef MCLIB_PROTOCOL_PROTOCOLSTATE_H_
#define MCLIB_PROTOCOL_PROTOCOLSTATE_H_

#include <common/VarInt.h>


enum class State
{
	Handshake = 0,
	Status = 1,
	Login = 2,
	Play = 3
};

enum class Version
{
	Minecraft_1_12_2 = 340,
};

MCLIB_API std::string to_string(Version version);

class UnfinishedProtocolException
{
private:
	CVarInt m_PacketId;
	State m_ProtocolState;

public:
	UnfinishedProtocolException(CVarInt id, State state) : m_PacketId(id), m_ProtocolState(state) {}
	std::string what() const
	{
		return "Unknown packet type " + std::to_string(m_PacketId.GetInt()) + " received during " + std::to_string((s32)m_ProtocolState) + " protocol state.";
	}
};


namespace login
{

enum ProtocolLogin
{
	Disconnect,
	EncryptionRequest,
	LoginSuccess,
	SetCompression
};

} // ns login

namespace status
{

enum ProtocolStatus
{
	Response = 0,
	Pong
};

} // ns status

namespace play
{

enum ProtocolPlay
{
	SpawnObject,
	SpawnExperienceOrb,
	SpawnGlobalEntity,
	SpawnMob,
	SpawnPainting,
	SpawnPlayer,
	Animation,
	Statistics,
	BlockBreakAnimation,
	UpdateBlockEntity,
	BlockAction,
	BlockChange,
	BossBar,
	ServerDifficulty,
	TabComplete,
	Chat,
	MultiBlockChange,
	ConfirmTransaction,
	CloseWindow,
	OpenWindow,
	WindowItems,
	WindowProperty,
	SetSlot,
	SetCooldown,
	PluginMessage,
	NamedSoundEffect,
	Disconnect,
	EntityStatus,
	Explosion,
	UnloadChunk,
	ChangeGameState,
	KeepAlive,
	ChunkData,
	Effect,
	Particle,
	JoinGame,
	Map,
	EntityRelativeMove,
	EntityLookAndRelativeMove,
	EntityLook,
	Entity,
	VehicleMove,
	OpenSignEditor,
	PlayerAbilities,
	CombatEvent,
	PlayerListItem,
	PlayerPositionAndLook,
	UseBed,
	DestroyEntities,
	RemoveEntityEffect,
	ResourcePackSend,
	Respawn,
	EntityHeadLook,
	WorldBorder,
	Camera,
	HeldItemChange,
	DisplayScoreboard,
	EntityMetadata,
	AttachEntity,
	EntityVelocity,
	EntityEquipment,
	SetExperience,
	UpdateHealth,
	ScoreboardObjective,
	SetPassengers,
	Teams,
	UpdateScore,
	SpawnPosition,
	TimeUpdate,
	Title,
	SoundEffect,
	PlayerListHeaderAndFooter,
	CollectItem,
	EntityTeleport,
	EntityProperties,
	EntityEffect,
	AdvancementProgress = 0x4E,

	Advancements,
	UnlockRecipes,
	CraftRecipeResponse,
};

} // ns play


namespace std
{

template <>
struct hash<State>
{
	std::size_t operator()(const State& state) const
	{
		return std::hash<int>()(static_cast<int>(state));
	}
};

template <>
struct hash<Version>
{
	std::size_t operator()(const Version& version) const
	{
		return std::hash<int>()(static_cast<int>(version));
	}
};

} // ns std

#endif
