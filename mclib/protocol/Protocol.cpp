#include "stdafx.h"

#include <protocol/Protocol.h>

#include <protocol/packets/Packet.h>
#include <protocol/packets/PacketsPlayInbound.h>

// Protocol agnostic protocol id to packet creators.
std::unordered_map<State, std::unordered_map<int32, PacketCreator>> agnosticStateMap = {
	{
		State::Login,
		{
			{ login::Disconnect, []() -> CMinecraftInboundPacket* { return new login::in::DisconnectPacket(); } },
			{ login::EncryptionRequest, []() -> CMinecraftInboundPacket* { return new login::in::EncryptionRequestPacket(); } },
			{ login::LoginSuccess, []() -> CMinecraftInboundPacket* { return new login::in::LoginSuccessPacket(); } },
			{ login::SetCompression, []() -> CMinecraftInboundPacket* { return new login::in::SetCompressionPacket(); } },
		}
	},
	{
		State::Status,
		{
			{ status::Response, []() -> CMinecraftInboundPacket* { return new status::in::ResponsePacket(); } },
			{ status::Pong, []() -> CMinecraftInboundPacket* { return new status::in::PongPacket(); } },
		}
	},
	{
		State::Play,
		{
			{ play::SpawnObject,                  []() -> CMinecraftInboundPacket* { return new in::SpawnObjectPacket(); } },
			{ play::SpawnExperienceOrb,           []() -> CMinecraftInboundPacket* { return new in::SpawnExperienceOrbPacket(); } },
			{ play::SpawnGlobalEntity,            []() -> CMinecraftInboundPacket* { return new in::SpawnGlobalEntityPacket(); } },
			{ play::SpawnMob,                     []() -> CMinecraftInboundPacket* { return new in::SpawnMobPacket(); } },
			{ play::SpawnPainting,                []() -> CMinecraftInboundPacket* { return new in::SpawnPaintingPacket(); } },
			{ play::SpawnPlayer,                  []() -> CMinecraftInboundPacket* { return new in::SpawnPlayerPacket(); } },
			{ play::Animation,                    []() -> CMinecraftInboundPacket* { return new in::AnimationPacket(); } },
			{ play::Statistics,                   []() -> CMinecraftInboundPacket* { return new in::StatisticsPacket(); } },
			{ play::BlockBreakAnimation,          []() -> CMinecraftInboundPacket* { return new in::BlockBreakAnimationPacket(); } },
			{ play::UpdateBlockEntity,            []() -> CMinecraftInboundPacket* { return new in::UpdateBlockEntityPacket(); } },
			{ play::BlockAction,                  []() -> CMinecraftInboundPacket* { return new in::BlockActionPacket(); } },
			{ play::BlockChange,                  []() -> CMinecraftInboundPacket* { return new in::BlockChangePacket(); } },
			{ play::BossBar,                      []() -> CMinecraftInboundPacket* { return new in::BossBarPacket(); } },
			{ play::ServerDifficulty,             []() -> CMinecraftInboundPacket* { return new in::ServerDifficultyPacket(); } },
			{ play::TabComplete,                  []() -> CMinecraftInboundPacket* { return new in::TabCompletePacket(); } },
			{ play::Chat,                         []() -> CMinecraftInboundPacket* { return new in::ChatPacket(); } },
			{ play::MultiBlockChange,             []() -> CMinecraftInboundPacket* { return new in::MultiBlockChangePacket(); } },
			{ play::ConfirmTransaction,           []() -> CMinecraftInboundPacket* { return new in::ConfirmTransactionPacket(); } },
			{ play::CloseWindow,                  []() -> CMinecraftInboundPacket* { return new in::CloseWindowPacket(); } },
			{ play::OpenWindow,                   []() -> CMinecraftInboundPacket* { return new in::OpenWindowPacket(); } },
			{ play::WindowItems,                  []() -> CMinecraftInboundPacket* { return new in::WindowItemsPacket(); } },
			{ play::WindowProperty,               []() -> CMinecraftInboundPacket* { return new in::WindowPropertyPacket(); } },
			{ play::SetSlot,                      []() -> CMinecraftInboundPacket* { return new in::SetSlotPacket(); } },
			{ play::SetCooldown,                  []() -> CMinecraftInboundPacket* { return new in::SetCooldownPacket(); } },
			{ play::PluginMessage,                []() -> CMinecraftInboundPacket* { return new in::PluginMessagePacket(); } },
			{ play::NamedSoundEffect,             []() -> CMinecraftInboundPacket* { return new in::NamedSoundEffectPacket(); } },
			{ play::Disconnect,                   []() -> CMinecraftInboundPacket* { return new login::in::DisconnectPacket(); } },
			{ play::EntityStatus,                 []() -> CMinecraftInboundPacket* { return new in::EntityStatusPacket(); } },
			{ play::Explosion,                    []() -> CMinecraftInboundPacket* { return new in::ExplosionPacket(); } },
			{ play::UnloadChunk,                  []() -> CMinecraftInboundPacket* { return new in::UnloadChunkPacket(); } },
			{ play::ChangeGameState,              []() -> CMinecraftInboundPacket* { return new in::ChangeGameStatePacket(); } },
			{ play::KeepAlive,                    []() -> CMinecraftInboundPacket* { return new in::KeepAlivePacket(); } },
			{ play::ChunkData,                    []() -> CMinecraftInboundPacket* { return new in::ChunkDataPacket(); } },
			{ play::Effect,                       []() -> CMinecraftInboundPacket* { return new in::EffectPacket(); } },
			{ play::Particle,                     []() -> CMinecraftInboundPacket* { return new in::ParticlePacket(); } },
			{ play::JoinGame,                     []() -> CMinecraftInboundPacket* { return new in::JoinGamePacket(); } },
			{ play::Map,                          []() -> CMinecraftInboundPacket* { return new in::MapPacket(); } },
			{ play::EntityRelativeMove,           []() -> CMinecraftInboundPacket* { return new in::EntityRelativeMovePacket(); } },
			{ play::EntityLookAndRelativeMove,    []() -> CMinecraftInboundPacket* { return new in::EntityLookAndRelativeMovePacket(); } },
			{ play::EntityLook,                   []() -> CMinecraftInboundPacket* { return new in::EntityLookPacket(); } },
			{ play::Entity,                       []() -> CMinecraftInboundPacket* { return new in::EntityPacket(); } },
			{ play::VehicleMove,                  []() -> CMinecraftInboundPacket* { return new in::VehicleMovePacket(); } },
			{ play::OpenSignEditor,               []() -> CMinecraftInboundPacket* { return new in::OpenSignEditorPacket(); } },
			{ play::PlayerAbilities,              []() -> CMinecraftInboundPacket* { return new in::PlayerAbilitiesPacket(); } },
			{ play::CombatEvent,                  []() -> CMinecraftInboundPacket* { return new in::CombatEventPacket(); } },
			{ play::PlayerListItem,               []() -> CMinecraftInboundPacket* { return new in::PlayerListItemPacket(); } },
			{ play::PlayerPositionAndLook,        []() -> CMinecraftInboundPacket* { return new in::PlayerPositionAndLookPacket(); } },
			{ play::UseBed,                       []() -> CMinecraftInboundPacket* { return new in::UseBedPacket(); } },
			{ play::DestroyEntities,              []() -> CMinecraftInboundPacket* { return new in::DestroyEntitiesPacket(); } },
			{ play::RemoveEntityEffect,           []() -> CMinecraftInboundPacket* { return new in::RemoveEntityEffectPacket(); } },
			{ play::ResourcePackSend,             []() -> CMinecraftInboundPacket* { return new in::ResourcePackSendPacket(); } },
			{ play::Respawn,                      []() -> CMinecraftInboundPacket* { return new in::RespawnPacket(); } },
			{ play::EntityHeadLook,               []() -> CMinecraftInboundPacket* { return new in::EntityHeadLookPacket(); } },
			{ play::WorldBorder,                  []() -> CMinecraftInboundPacket* { return new in::WorldBorderPacket(); } },
			{ play::Camera,                       []() -> CMinecraftInboundPacket* { return new in::CameraPacket(); } },
			{ play::HeldItemChange,               []() -> CMinecraftInboundPacket* { return new in::HeldItemChangePacket(); } },
			{ play::DisplayScoreboard,            []() -> CMinecraftInboundPacket* { return new in::DisplayScoreboardPacket(); } },
			{ play::EntityMetadata,               []() -> CMinecraftInboundPacket* { return new in::EntityMetadataPacket(); } },
			{ play::AttachEntity,                 []() -> CMinecraftInboundPacket* { return new in::AttachEntityPacket(); } },
			{ play::EntityVelocity,               []() -> CMinecraftInboundPacket* { return new in::EntityVelocityPacket(); } },
			{ play::EntityEquipment,              []() -> CMinecraftInboundPacket* { return new in::EntityEquipmentPacket(); } },
			{ play::SetExperience,                []() -> CMinecraftInboundPacket* { return new in::SetExperiencePacket(); } },
			{ play::UpdateHealth,                 []() -> CMinecraftInboundPacket* { return new in::UpdateHealthPacket(); } },
			{ play::ScoreboardObjective,          []() -> CMinecraftInboundPacket* { return new in::ScoreboardObjectivePacket(); } },
			{ play::SetPassengers,                []() -> CMinecraftInboundPacket* { return new in::SetPassengersPacket(); } },
			{ play::Teams,                        []() -> CMinecraftInboundPacket* { return new in::TeamsPacket(); } },
			{ play::UpdateScore,                  []() -> CMinecraftInboundPacket* { return new in::UpdateScorePacket(); } },
			{ play::SpawnPosition,                []() -> CMinecraftInboundPacket* { return new in::SpawnPositionPacket(); } },
			{ play::TimeUpdate,                   []() -> CMinecraftInboundPacket* { return new in::TimeUpdatePacket(); } },
			{ play::Title,                        []() -> CMinecraftInboundPacket* { return new in::TitlePacket(); } },
			{ play::SoundEffect,                  []() -> CMinecraftInboundPacket* { return new in::SoundEffectPacket(); } },
			{ play::PlayerListHeaderAndFooter,    []() -> CMinecraftInboundPacket* { return new in::PlayerListHeaderAndFooterPacket(); } },
			{ play::CollectItem,                  []() -> CMinecraftInboundPacket* { return new in::CollectItemPacket(); } },
			{ play::EntityTeleport,               []() -> CMinecraftInboundPacket* { return new in::EntityTeleportPacket(); } },
			{ play::EntityProperties,             []() -> CMinecraftInboundPacket* { return new in::EntityPropertiesPacket(); } },
			{ play::EntityEffect,                 []() -> CMinecraftInboundPacket* { return new in::EntityEffectPacket(); } },
			{ play::AdvancementProgress,          []() -> CMinecraftInboundPacket* { return new in::AdvancementProgressPacket(); } },
			{ play::Advancements,                 []() -> CMinecraftInboundPacket* { return new in::AdvancementsPacket(); } },
			{ play::UnlockRecipes,                []() -> CMinecraftInboundPacket* { return new in::UnlockRecipesPacket(); } },
		}
	}
};


Protocol::StateMap inboundMap_1_12_1 = {
	{
		State::Login,
		{
			{ 0x00, login::Disconnect },
			{ 0x01, login::EncryptionRequest },
			{ 0x02, login::LoginSuccess },
			{ 0x03, login::SetCompression },
		}
	},
	{
		State::Status,
		{
			{ 0x00, status::Response },
			{ 0x01, status::Pong },
		}
	},
	{
		State::Play,
		{
			{ 0x00, play::SpawnObject },
			{ 0x01, play::SpawnExperienceOrb },
			{ 0x02, play::SpawnGlobalEntity },
			{ 0x03, play::SpawnMob },
			{ 0x04, play::SpawnPainting },
			{ 0x05, play::SpawnPlayer },
			{ 0x06, play::Animation },
			{ 0x07, play::Statistics },
			{ 0x08, play::BlockBreakAnimation },
			{ 0x09, play::UpdateBlockEntity },
			{ 0x0A, play::BlockAction },
			{ 0x0B, play::BlockChange },
			{ 0x0C, play::BossBar },
			{ 0x0D, play::ServerDifficulty },
			{ 0x0E, play::TabComplete },
			{ 0x0F, play::Chat },
			{ 0x10, play::MultiBlockChange },
			{ 0x11, play::ConfirmTransaction },
			{ 0x12, play::CloseWindow },
			{ 0x13, play::OpenWindow },
			{ 0x14, play::WindowItems },
			{ 0x15, play::WindowProperty },
			{ 0x16, play::SetSlot },
			{ 0x17, play::SetCooldown },
			{ 0x18, play::PluginMessage },
			{ 0x19, play::NamedSoundEffect },
			{ 0x1A, play::Disconnect },
			{ 0x1B, play::EntityStatus },
			{ 0x1C, play::Explosion },
			{ 0x1D, play::UnloadChunk },
			{ 0x1E, play::ChangeGameState },
			{ 0x1F, play::KeepAlive },
			{ 0x20, play::ChunkData },
			{ 0x21, play::Effect },
			{ 0x22, play::Particle },
			{ 0x23, play::JoinGame },
			{ 0x24, play::Map },
			{ 0x25, play::Entity },
			{ 0x26, play::EntityRelativeMove },
			{ 0x27, play::EntityLookAndRelativeMove },
			{ 0x28, play::EntityLook },
			{ 0x29, play::VehicleMove },
			{ 0x2A, play::OpenSignEditor },

			{ 0x2B, play::CraftRecipeResponse },

			{ 0x2C, play::PlayerAbilities },
			{ 0x2D, play::CombatEvent },
			{ 0x2E, play::PlayerListItem },
			{ 0x2F, play::PlayerPositionAndLook },
			{ 0x30, play::UseBed },
			{ 0x31, play::UnlockRecipes },
			{ 0x32, play::DestroyEntities },
			{ 0x33, play::RemoveEntityEffect },
			{ 0x34, play::ResourcePackSend },
			{ 0x35, play::Respawn },
			{ 0x36, play::EntityHeadLook },
			{ 0x37, play::AdvancementProgress },
			{ 0x38, play::WorldBorder },
			{ 0x39, play::Camera },
			{ 0x3A, play::HeldItemChange },
			{ 0x3B, play::DisplayScoreboard },
			{ 0x3C, play::EntityMetadata },
			{ 0x3D, play::AttachEntity },
			{ 0x3E, play::EntityVelocity },
			{ 0x3F, play::EntityEquipment },
			{ 0x40, play::SetExperience },
			{ 0x41, play::UpdateHealth },
			{ 0x42, play::ScoreboardObjective },
			{ 0x43, play::SetPassengers },
			{ 0x44, play::Teams },
			{ 0x45, play::UpdateScore },
			{ 0x46, play::SpawnPosition },
			{ 0x47, play::TimeUpdate },
			{ 0x48, play::Title },
			{ 0x49, play::SoundEffect },
			{ 0x4A, play::PlayerListHeaderAndFooter },
			{ 0x4B, play::CollectItem },
			{ 0x4C, play::EntityTeleport },
			{ 0x4D, play::Advancements },
			{ 0x4E, play::EntityProperties },
			{ 0x4F, play::EntityEffect },
		}
	}
};


const std::unordered_map<Version, std::shared_ptr<Protocol>> protocolMap = {
	{ Version::Minecraft_1_12_2, std::make_shared<Protocol>(Version::Minecraft_1_12_2, inboundMap_1_12_1) },
};

bool Protocol::GetAgnosticId(State state, int32 protocolId, int32& agnosticId)
{
	auto& packetMap = m_InboundMap[state];
	auto iter = packetMap.find(protocolId);

	if (iter == packetMap.end())
		return false;

	agnosticId = iter->second;

	return true;
}

CMinecraftInboundPacket* Protocol::CreateInboundPacket(State state, int32 protocolId)
{
	int32 agnosticId = 0;

	if (false == GetAgnosticId(state, protocolId, agnosticId))
	{
		Log::Error("Protocol::CreateInboundPacket: Unable to GetAgnosticId for State = '%d' and ID = '%d'.", state, protocolId);
		return nullptr;
	}

	auto stateMap = agnosticStateMap[state];
	auto iter = stateMap.find(agnosticId);
	if (iter != stateMap.end())
	{
		if (CMinecraftInboundPacket* packet = iter->second())
		{
			packet->SetId(protocolId);
			packet->SetProtocolVersion(m_Version);
			return packet;
		}
	}

	Log::Error("Protocol::CreateInboundPacket: Unable to stateMap for State = '%d' and ID = '%d'.", state, protocolId);
	return nullptr;
}

Protocol& Protocol::GetProtocol(Version version)
{
	auto&& iter = protocolMap.find(version);

	if (iter == protocolMap.end())
	{
		throw std::runtime_error(std::string("Unknown protocol version ") + std::to_string((int32)version));
	}

	return *iter->second;
}

std::string to_string(Version version)
{
	static const std::unordered_map<Version, std::string> mapping = {
		{ Version::Minecraft_1_12_2, "1.12.2" },
	};

	auto iter = mapping.find(version);
	if (iter == mapping.end()) return "Unknown";
	return iter->second;
}