#include "stdafx.h"

#include <protocol/packets/PacketsPlayInbound.h>

#include <core/Connection.h>
#include <protocol/PacketHandler.h>

namespace in
{
// Play packets
SpawnObjectPacket::SpawnObjectPacket()
{

}
bool SpawnObjectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid;
	m_EntityId = eid.GetInt();

	data >> m_UUID;
	data >> m_Type;

	double x, y, z;

	data >> x >> y >> z;

	m_Position = glm::vec3((float)x, (float)y, (float)z);

	data >> m_Pitch >> m_Yaw;

	data >> m_Data;
	data >> m_Velocity.x >> m_Velocity.y >> m_Velocity.z;

	return true;
}
void SpawnObjectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SpawnExperienceOrbPacket::SpawnExperienceOrbPacket()
{

}
bool SpawnExperienceOrbPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	double x, y, z;

	data >> eid >> x >> y >> z >> m_Count;

	m_EntityId = eid.GetInt();
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;

	return true;
}
void SpawnExperienceOrbPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SpawnGlobalEntityPacket::SpawnGlobalEntityPacket()
{}

bool SpawnGlobalEntityPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	data >> eid >> m_Type;

	double x, y, z;
	data >> x >> y >> z;
	m_Position = glm::dvec3(x, y, z);

	return true;
}
void SpawnGlobalEntityPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SpawnMobPacket::SpawnMobPacket() 
	: CMinecraftInboundPacket()
	, m_Metadata(m_ProtocolVersion)
{}

bool SpawnMobPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt entityId, type;

	m_Metadata.SetProtocolVersion(m_ProtocolVersion);

	data >> entityId;
	m_EntityId = entityId.GetInt();
	data >> m_UUID;

	data >> type;
	m_Type = type.GetInt();

	double x, y, z;
	data >> x >> y >> z;
	m_Position = glm::dvec3(x, y, z);

	data >> m_Yaw;
	data >> m_Pitch;
	data >> m_HeadPitch;

	short vx, vy, vz;
	data >> vx >> vy >> vz;
	m_Velocity = glm::i16vec3(vx, vy, vz);
	data >> m_Metadata;

	return true;
}
void SpawnMobPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SpawnPaintingPacket::SpawnPaintingPacket()
{

}
bool SpawnPaintingPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	MCString title;
	Position position;
	uint8 direction;

	data >> eid >> m_UUID;
	data >> title;
	data >> position >> direction;

	m_EntityId = eid.GetInt();
	m_Title = title.GetUTF16();
	m_Position.x = position.GetX();
	m_Position.y = position.GetY();
	m_Position.z = position.GetZ();
	m_Direction = (Direction)direction;

	return true;
}
void SpawnPaintingPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SpawnPlayerPacket::SpawnPlayerPacket() : CMinecraftInboundPacket(), m_Metadata(m_ProtocolVersion)
{

}
bool SpawnPlayerPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	m_Metadata.SetProtocolVersion(m_ProtocolVersion);

	data >> eid;
	m_EntityId = eid.GetInt();

	data >> m_UUID;

	double x, y, z;
	data >> x >> y >> z;

	m_Position = glm::dvec3(x, y, z);

	data >> m_Yaw;
	data >> m_Pitch;

	data >> m_Metadata;
	return true;
}
void SpawnPlayerPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


AnimationPacket::AnimationPacket()
{

}
bool AnimationPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid;

	m_EntityId = eid.GetInt();

	uint8 anim;
	data >> anim;

	m_Animation = (Animation)anim;
	return true;
}
void AnimationPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


StatisticsPacket::StatisticsPacket()
{

}
bool StatisticsPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt count;
	data >> count;

	for (int32 i = 0; i < count.GetInt(); ++i)
	{
		MCString name;
		CVarInt value;

		data >> name;
		data >> value;

		m_Statistics[name.GetUTF16()] = value.GetInt();
	}

	return true;
}
void StatisticsPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


AdvancementsPacket::AdvancementsPacket()
{

}
bool AdvancementsPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Reset;

	CVarInt mappingSize;

	data >> mappingSize;

	for (int32 i = 0; i < mappingSize.GetInt(); ++i)
	{
		MCString key;
		Advancement advancement;

		data >> key;

		bool hasParent;
		data >> hasParent;

		if (hasParent)
		{
			MCString parent;
			data >> parent;
			advancement.parentId = parent.GetUTF16();
		}

		bool hasDisplay;
		data >> hasDisplay;
		if (hasDisplay)
		{
			MCString title, description;
			data >> title >> description;

			advancement.display.title = title.GetUTF16();
			advancement.display.description = description.GetUTF16();

			advancement.display.icon.Deserialize(data, m_ProtocolVersion);

			CVarInt frameType;
			data >> frameType;

			advancement.display.frameType = (FrameType)frameType.GetInt();
			data >> advancement.display.flags;

			if ((advancement.display.flags & (int32)Flags::BackgroundTexture) != 0)
			{
				MCString texture;
				data >> texture;

				advancement.display.backgroundTexture = texture.GetUTF16();
			}

			data >> advancement.display.x >> advancement.display.y;
		}

		CVarInt criteriaSize;
		data >> criteriaSize;

		for (int32 j = 0; j < criteriaSize.GetInt(); ++j)
		{
			MCString identifier;
			data >> identifier;
		}

		CVarInt requirementSize;
		data >> requirementSize;

		advancement.requirements.resize(requirementSize.GetInt());

		for (int32 j = 0; j < requirementSize.GetInt(); ++j)
		{
			CVarInt size;
			data >> size;

			auto& requirementArray = advancement.requirements[j];
			requirementArray.resize(size.GetInt());

			for (int32 k = 0; k < size.GetInt(); ++k)
			{
				MCString requirement;

				data >> requirement;

				requirementArray[k] = requirement.GetUTF16();
			}
		}

		m_Advancements.insert(std::make_pair(key.GetUTF16(), advancement));
	}

	CVarInt listSize;
	data >> listSize;

	for (int32 i = 0; i < listSize.GetInt(); ++i)
	{
		MCString identifier;
		data >> identifier;

		m_RemoveIdentifiers.push_back(identifier.GetUTF16());
	}

	CVarInt progressSize;
	data >> progressSize;

	for (int32 i = 0; i < progressSize.GetInt(); ++i)
	{
		// The identifier of the advancement
		MCString key;

		data >> key;

		CVarInt size;
		data >> size;

		AdvancementProgress progress;
		for (int32 j = 0; j < size.GetInt(); ++j)
		{
			MCString criterionIdentifier;
			CriterionProgress criterionProgress;

			data >> criterionIdentifier;

			data >> criterionProgress.achieved;
			if (criterionProgress.achieved)
				data >> criterionProgress.date;

			progress.insert(std::make_pair(criterionIdentifier.GetUTF16(), criterionProgress));
		}

		m_Progress.insert(std::make_pair(key.GetUTF16(), progress));
	}

	return true;
}
void AdvancementsPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


BlockBreakAnimationPacket::BlockBreakAnimationPacket()
{

}
bool BlockBreakAnimationPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	Position position;

	data >> eid >> position >> m_DestroyStage;

	m_EntityId = eid.GetInt();
	m_Position.x = position.GetX();
	m_Position.y = position.GetY();
	m_Position.z = position.GetZ();

	return true;
}
void BlockBreakAnimationPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


UpdateBlockEntityPacket::UpdateBlockEntityPacket()
{

}
bool UpdateBlockEntityPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	Position pos;
	uint8 action;

	data >> pos;
	data >> action;

	NBT nbt;
	data >> nbt;

	m_BlockEntity = BlockEntity::CreateFromNBT(&nbt);
	m_Position = glm::ivec3(pos.GetX(), pos.GetY(), pos.GetZ());
	m_Action = (Action)action;

	return true;
}
void UpdateBlockEntityPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


BlockActionPacket::BlockActionPacket()
{

}
bool BlockActionPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	Position position;
	CVarInt type;

	data >> position >> m_ActionId >> m_ActionParam >> type;

	m_Position.x = position.GetX();
	m_Position.y = position.GetY();
	m_Position.z = position.GetZ();
	m_BlockType = type.GetInt();

	return true;
}
void BlockActionPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


BlockChangePacket::BlockChangePacket()
{

}
bool BlockChangePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	Position location;
	CVarInt blockId;

	data >> location >> blockId;

	m_Position = glm::ivec3(location.GetX(), location.GetY(), location.GetZ());
	m_BlockId = blockId.GetInt();

	return true;
}
void BlockChangePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


BossBarPacket::BossBarPacket()
{

}
bool BossBarPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	static int DivisionCounts[] = { 0, 6, 10, 12, 20 };
	CVarInt action;

	data >> m_UUID >> action;
	m_Action = (Action)action.GetInt();

	switch (m_Action)
	{
		case Action::Add:
		{
			MCString title;
			CVarInt color, divisions;

			data >> title >> m_Health >> color >> divisions >> m_Flags;

			m_Title = title.GetUTF16();
			m_Color = (Color)color.GetInt();
			m_Divisions = DivisionCounts[divisions.GetInt()];
		}
		break;
		case Action::Remove:
		{
		}
		break;
		case Action::UpdateHealth:
		{
			data >> m_Health;
		}
		break;
		case Action::UpdateTitle:
		{
			MCString title;

			data >> title;
			m_Title = title.GetUTF16();
		}
		break;
		case Action::UpdateStyle:
		{
			CVarInt color, divisions;

			data >> color >> divisions;

			m_Color = (Color)color.GetInt();
			m_Divisions = DivisionCounts[divisions.GetInt()];
		}
		break;
		case Action::UpdateFlags:
		{
			data >> m_Flags;
		}
		break;
	}

	return true;
}
void BossBarPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ServerDifficultyPacket::ServerDifficultyPacket()
{

}
bool ServerDifficultyPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Difficulty;
	return true;
}
void ServerDifficultyPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


TabCompletePacket::TabCompletePacket()
{

}
bool TabCompletePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt count;
	data >> count;
	for (int32 i = 0; i < count.GetInt(); ++i)
	{
		MCString match;
		data >> match;
		m_Matches.push_back(match.GetUTF16());
	}

	return true;
}
void TabCompletePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ChatPacket::ChatPacket()
{

}
bool ChatPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString chatData;
	uint8 position;

	data >> chatData;
	data >> position;

	m_Position = (ChatPosition)position;

	try
	{
		m_ChatData = json::parse(chatData.GetUTF8());
	}
	catch (json::parse_error&)
	{

	}

	return true;
}
void ChatPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


MultiBlockChangePacket::MultiBlockChangePacket()
{

}
bool MultiBlockChangePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_ChunkX >> m_ChunkZ;

	CVarInt count;
	data >> count;
	for (int32 i = 0; i < count.GetInt(); ++i)
	{
		uint8 horizontal;
		uint8 y;
		CVarInt blockID;

		data >> horizontal >> y >> blockID;

		BlockChange change;
		change.x = horizontal >> 4;
		change.z = horizontal & 15;
		change.y = y;
		change.blockData = blockID.GetShort();

		m_BlockChanges.push_back(change);
	}
	return true;
}
void MultiBlockChangePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ConfirmTransactionPacket::ConfirmTransactionPacket()
{

}
bool ConfirmTransactionPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_WindowId >> m_Action >> m_Accepted;
	return true;
}
void ConfirmTransactionPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


CloseWindowPacket::CloseWindowPacket()
{

}
bool CloseWindowPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_WindowId;
	return true;
}
void CloseWindowPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


OpenWindowPacket::OpenWindowPacket()
{

}
bool OpenWindowPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString type, title;

	data >> m_WindowId >> type >> title >> m_SlotCount;

	m_WindowType = type.GetUTF16();
	m_WindowTitle = title.GetUTF16();

	m_EntityId = 0;
	if (m_WindowType.compare(L"EntityHorse") == 0)
		data >> m_EntityId;

	return true;
}
void OpenWindowPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


WindowItemsPacket::WindowItemsPacket()
{

}
bool WindowItemsPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_WindowId;
	int16 count;
	data >> count;

	for (int16 i = 0; i < count; ++i)
	{
		Slot slot;

		slot.Deserialize(data, m_ProtocolVersion);

		m_Slots.push_back(slot);
	}

	return true;
}
void WindowItemsPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


WindowPropertyPacket::WindowPropertyPacket()
{

}
bool WindowPropertyPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_WindowId >> m_Property >> m_Value;
	return true;
}
void WindowPropertyPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SetSlotPacket::SetSlotPacket()
{

}
bool SetSlotPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_WindowId;
	data >> m_SlotIndex;
	m_Slot.Deserialize(data, m_ProtocolVersion);
	return true;
}
void SetSlotPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SetCooldownPacket::SetCooldownPacket()
{

}
bool SetCooldownPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt item, ticks;
	data >> item >> ticks;

	m_ItemId = item.GetInt();
	m_Ticks = ticks.GetInt();

	return true;
}
void SetCooldownPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


PluginMessagePacket::PluginMessagePacket()
{

}
bool PluginMessagePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	std::size_t begin = data.GetReadOffset();

	data >> m_Channel;

	data.ReadSome(m_Data, data.GetSize() - (data.GetReadOffset() - begin) - 1);

	return true;
}
void PluginMessagePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


NamedSoundEffectPacket::NamedSoundEffectPacket()
{

}
bool NamedSoundEffectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString name;
	CVarInt category;

	data >> name >> category;

	m_Name = name.GetUTF16();
	m_Category = (SoundCategory)category.GetInt();

	FixedPointNumber<uint32> x, y, z;

	data >> x >> y >> z;
	m_Position.x = (double)x.GetFloat();
	m_Position.y = (double)y.GetFloat();
	m_Position.z = (double)z.GetFloat();

	data >> m_Volume >> m_Pitch;

	return true;
}
void NamedSoundEffectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityStatusPacket::EntityStatusPacket()
{

}
bool EntityStatusPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_EntityId;
	data >> m_Status;
	return true;
}
void EntityStatusPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ExplosionPacket::ExplosionPacket()
{

}
bool ExplosionPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	float posX, posY, posZ;
	int32 count;

	data >> posX >> posY >> posZ;

	m_Position = glm::dvec3(posX, posY, posZ);

	data >> m_Radius;
	data >> count;

	for (int32 i = 0; i < count; ++i)
	{
		int8 x, y, z;
		data >> x >> y >> z;
		m_AffectedBlocks.push_back(glm::i16vec3(x, y, z));
	}

	float motionX, motionY, motionZ;

	data >> motionX >> motionY >> motionZ;

	m_PlayerMotion = glm::dvec3(motionX, motionY, motionZ);

	return true;
}
void ExplosionPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


UnloadChunkPacket::UnloadChunkPacket()
{

}
bool UnloadChunkPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_ChunkX >> m_ChunkZ;
	return true;
}
void UnloadChunkPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ChangeGameStatePacket::ChangeGameStatePacket()
{

}
bool ChangeGameStatePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	uint8 reason;
	data >> reason;

	m_Reason = (Reason)reason;

	data >> m_Value;

	return true;
}
void ChangeGameStatePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


KeepAlivePacket::KeepAlivePacket()
{

}
bool KeepAlivePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_AliveId;
	return true;
}
void KeepAlivePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ChunkDataPacket::ChunkDataPacket()
{

}
bool ChunkDataPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	ChunkColumnMetadata metadata;

	data >> metadata.x;
	data >> metadata.z;
	data >> metadata.continuous;
	CVarInt mask;
	data >> mask;

	metadata.sectionmask = mask.GetInt();

	if (m_Connection)
		metadata.skylight = m_Connection->GetDimension() == 0;
	else
		metadata.skylight = true;

	CVarInt size;

	data >> size;

	m_ChunkColumn = std::make_shared<ChunkColumn>(metadata);

	data >> *m_ChunkColumn;

	// Skip biome information
	if (metadata.continuous)
		data.SetReadOffset(data.GetReadOffset() + 256);

	CVarInt entities;
	data >> entities;

	int32 entityCount = entities.GetInt();

	for (int32 i = 0; i < entities.GetInt(); ++i)
	{
		NBT nbt;

		data >> nbt;

		BlockEntityPtr blockEntity = BlockEntity::CreateFromNBT(&nbt);

		if (blockEntity == nullptr) continue;

		m_BlockEntities.push_back(blockEntity);
		m_ChunkColumn->AddBlockEntity(blockEntity);
	}

	return true;
}
void ChunkDataPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EffectPacket::EffectPacket()
{

}
bool EffectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_EffectId;
	Position pos;
	data >> pos;

	m_Position.x = pos.GetX();
	m_Position.y = pos.GetY();
	m_Position.z = pos.GetZ();

	data >> m_Data;
	data >> m_DisableRelativeVolume;

	return true;
}
void EffectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


ParticlePacket::ParticlePacket()
{

}
bool ParticlePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_ParticleId >> m_LongDistance;

	float x, y, z;
	data >> x >> y >> z;
	m_Position = glm::dvec3(x, y, z);

	float offsetX, offsetY, offsetZ;
	data >> offsetX >> offsetY >> offsetZ;
	m_MaxOffset = glm::dvec3(offsetX, offsetY, offsetZ);

	data >> m_ParticleData >> m_Count;

	if (m_ParticleId == 36)
	{ // iconcrack
		for (int32 i = 0; i < 2; ++i)
		{
			CVarInt varData;
			data >> varData;
			m_Data.push_back(varData.GetInt());
		}
	}
	else if (m_ParticleId == 37 || m_ParticleId == 38)
	{ // blockcrack || blockdust
		CVarInt varData;
		data >> varData;
		m_Data.push_back(varData.GetInt());
	}

	return true;
}
void ParticlePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


JoinGamePacket::JoinGamePacket()
{

}
bool JoinGamePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_EntityId;
	data >> m_Gamemode;
	data >> m_Dimension;
	data >> m_Difficulty;
	data >> m_MaxPlayers;
	data >> m_LevelType;
	data >> m_ReducedDebug;
	return true;
}
void JoinGamePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


MapPacket::MapPacket()
{

}
bool MapPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt mapId, count;

	data >> mapId >> m_Scale >> m_TrackPosition >> count;
	m_MapId = mapId.GetInt();

	for (int32 i = 0; i < count.GetInt(); ++i)
	{
		Icon icon;
		data >> icon.direction >> icon.x >> icon.z;

		icon.type = icon.direction & 0x0F;
		icon.direction = (icon.direction & 0xF0) >> 4;

		m_Icons.push_back(icon);
	}

	data >> m_Columns;

	if (m_Columns > 0)
	{
		CVarInt length;

		data >> m_Rows >> m_X >> m_Z >> length;
		data.ReadSome(m_Data, length.GetInt());
	}

	return true;
}
void MapPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityRelativeMovePacket::EntityRelativeMovePacket()
{

}
bool EntityRelativeMovePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid;
	data >> m_Delta.x >> m_Delta.y >> m_Delta.z;
	data >> m_OnGround;

	m_EntityId = eid.GetInt();

	return true;
}
void EntityRelativeMovePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityLookAndRelativeMovePacket::EntityLookAndRelativeMovePacket()
{

}
bool EntityLookAndRelativeMovePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid >> m_Delta.x >> m_Delta.y >> m_Delta.z;
	data >> m_Yaw >> m_Pitch;
	data >> m_OnGround;

	m_EntityId = eid.GetInt();

	return true;
}
void EntityLookAndRelativeMovePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityLookPacket::EntityLookPacket()
{

}
bool EntityLookPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid;

	m_EntityId = eid.GetInt();

	data >> m_Yaw >> m_Pitch;
	data >> m_OnGround;
	return true;
}
void EntityLookPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityPacket::EntityPacket()
{

}
bool EntityPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	data >> eid;
	m_EntityId = eid.GetInt();
	return true;
}
void EntityPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


VehicleMovePacket::VehicleMovePacket()
{

}
bool VehicleMovePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Position.x >> m_Position.y >> m_Position.z >> m_Yaw >> m_Pitch;
	return true;
}
void VehicleMovePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


OpenSignEditorPacket::OpenSignEditorPacket()
{

}
bool OpenSignEditorPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	Position position;

	data >> position;

	m_Position.x = position.GetX();
	m_Position.y = position.GetY();
	m_Position.z = position.GetZ();
	return true;
}
void OpenSignEditorPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


PlayerAbilitiesPacket::PlayerAbilitiesPacket()
{

}
bool PlayerAbilitiesPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Flags;
	data >> m_FlyingSpeed;
	data >> m_FOVModifier;

	return true;
}
void PlayerAbilitiesPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


CombatEventPacket::CombatEventPacket()
{

}
bool CombatEventPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt event;

	data >> event;

	m_Event = (Event)event.GetInt();

	if (m_Event == Event::EndCombat)
	{
		CVarInt duration;

		data >> duration;

		m_Duration = duration.GetInt();
		data >> m_EntityId;
	}
	else if (m_Event == Event::EntityDead)
	{
		CVarInt pid;
		data >> pid;

		m_PlayerId = pid.GetInt();
		data >> m_EntityId;

		MCString message;
		data >> message;

		m_Message = message.GetUTF16();
	}

	return true;
}
void CombatEventPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


PlayerListItemPacket::PlayerListItemPacket()
{

}
bool PlayerListItemPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt action;
	CVarInt numPlayers;

	data >> action;
	data >> numPlayers;

	m_Action = (Action)action.GetInt();

	for (int32 i = 0; i < numPlayers.GetInt(); ++i)
	{
		CUUID uuid;
		data >> uuid;

		ActionDataPtr actionData = std::make_shared<ActionData>();
		actionData->uuid = uuid;



		switch (m_Action)
		{
			case Action::AddPlayer:
			{
				MCString name;
				CVarInt numProperties;

				data >> name;
				data >> numProperties;

				actionData->name = name.GetUTF16();

				for (int32 j = 0; j < numProperties.GetInt(); ++j)
				{
					MCString propertyName;
					MCString propertyValue;
					uint8 isSigned;
					MCString signature;

					data >> propertyName;
					data >> propertyValue;
					data >> isSigned;
					if (isSigned)
						data >> signature;

					actionData->properties[propertyName.GetUTF16()] = propertyValue.GetUTF16();
				}

				CVarInt gameMode, ping;
				data >> gameMode;
				data >> ping;

				uint8 hasDisplayName;
				MCString displayName;

				data >> hasDisplayName;
				if (hasDisplayName)
					data >> displayName;

				actionData->gamemode = gameMode.GetInt();
				actionData->ping = ping.GetInt();
				actionData->displayName = displayName.GetUTF16();
			}
			break;
			case Action::UpdateGamemode:
			{
				CVarInt gameMode;
				data >> gameMode;

				actionData->gamemode = gameMode.GetInt();
			}
			break;
			case Action::UpdateLatency:
			{
				CVarInt ping;
				data >> ping;

				actionData->ping = ping.GetInt();
			}
			break;
			case Action::UpdateDisplay:
			{
				uint8 hasDisplayName;
				MCString displayName;

				data >> hasDisplayName;
				if (hasDisplayName)
					data >> displayName;

				actionData->displayName = displayName.GetUTF16();
			}
			break;
			case Action::RemovePlayer:
			{
				std::shared_ptr<ActionData> actionData = std::make_shared<ActionData>();
				actionData->uuid = uuid;
			}
			break;
		}

		m_Data.push_back(actionData);
	}

	return true;
}
void PlayerListItemPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


PlayerPositionAndLookPacket::PlayerPositionAndLookPacket()
{

}
bool PlayerPositionAndLookPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Position.x >> m_Position.y >> m_Position.z;
	data >> m_Yaw >> m_Pitch;
	data >> m_Flags;
	CVarInt tid;
	data >> tid;

	m_TeleportId = tid.GetInt();
	return true;
}
void PlayerPositionAndLookPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


UseBedPacket::UseBedPacket()
{

}
bool UseBedPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	Position location;

	data >> eid >> location;

	m_EntityId = eid.GetInt();
	m_Position.x = location.GetX();
	m_Position.y = location.GetY();
	m_Position.z = location.GetZ();
	return true;
}
void UseBedPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



DestroyEntitiesPacket::DestroyEntitiesPacket()
{}
bool DestroyEntitiesPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt count;

	data >> count;

	for (int32 i = 0; i < count.GetInt(); ++i)
	{
		CVarInt eid;

		data >> eid;

		m_EntityIds.push_back(eid.GetInt());
	}
	return true;
}
void DestroyEntitiesPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



UnlockRecipesPacket::UnlockRecipesPacket()
{}
bool UnlockRecipesPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	int16 action;

	data >> action;
	m_Action = (Action)action;

	data >> m_OpenCraftingBook;
	data >> m_Filter;

	CVarInt size1;
	data >> size1;

	m_Array1.resize(size1.GetInt());

	for (int32 i = 0; i < size1.GetInt(); ++i)
	{
		CVarInt id;

		data >> id;

		m_Array1[i] = id.GetInt();
	}

	if (m_Action == Action::Initialize)
	{
		CVarInt size2;
		data >> size2;

		m_Array2.resize(size2.GetInt());

		for (int32 i = 0; i < size2.GetInt(); ++i)
		{
			CVarInt id;

			data >> id;

			m_Array2[i] = id.GetInt();
		}
	}

	return true;
}
void UnlockRecipesPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



RemoveEntityEffectPacket::RemoveEntityEffectPacket()
{}
bool RemoveEntityEffectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	data >> eid >> m_EffectId;
	m_EntityId = eid.GetInt();
	return true;
}
void RemoveEntityEffectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



ResourcePackSendPacket::ResourcePackSendPacket()
{}
bool ResourcePackSendPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString url, hash;

	data >> url >> hash;

	m_Url = url.GetUTF16();
	m_Hash = hash.GetUTF8();

	return true;
}
void ResourcePackSendPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



RespawnPacket::RespawnPacket()
{}
bool RespawnPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Dimension;
	data >> m_Difficulty;
	data >> m_Gamemode;

	MCString level;
	data >> level;

	m_Level = level.GetUTF16();

	return true;
}
void RespawnPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



EntityHeadLookPacket::EntityHeadLookPacket()
{}
bool EntityHeadLookPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	data >> eid;
	data >> m_Yaw;
	m_EntityId = eid.GetInt();
	return true;
}
void EntityHeadLookPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



WorldBorderPacket::WorldBorderPacket()
{}
bool WorldBorderPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt action;

	data >> action;

	m_Action = (Action)action.GetInt();

	switch (m_Action)
	{
		case Action::SetSize:
		{
			data >> m_Diameter;
		}
		break;
		case Action::LerpSize:
		{
			data >> m_OldDiameter;
			data >> m_Diameter;
			VarLong speed;

			data >> speed;
			m_Speed = speed.GetLong();
		}
		break;
		case Action::SetCenter:
		{
			data >> m_X >> m_Z;
		}
		break;
		case Action::Initialize:
		{
			data >> m_X >> m_Z;

			data >> m_OldDiameter >> m_Diameter;

			VarLong speed;
			data >> speed;
			m_Speed = speed.GetLong();

			CVarInt portalTeleportBoundary, warningTime, warningBlocks;

			data >> portalTeleportBoundary >> warningTime >> warningBlocks;

			m_PortalTeleportBoundary = portalTeleportBoundary.GetInt();
			m_WarningTime = warningTime.GetInt();
			m_WarningBlocks = warningBlocks.GetInt();
		}
		break;
		case Action::SetWarningTime:
		{
			CVarInt warningTime;
			data >> warningTime;
			m_WarningTime = warningTime.GetInt();
		}
		break;
		case Action::SetWarningBlocks:
		{
			CVarInt warningBlocks;
			data >> warningBlocks;
			m_WarningBlocks = warningBlocks.GetInt();
		}
		break;
	}
	return true;
}
void WorldBorderPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



CameraPacket::CameraPacket()
{}
bool CameraPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt id;
	data >> id;
	m_EntityId = id.GetInt();
	return true;
}
void CameraPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



HeldItemChangePacket::HeldItemChangePacket()
{}
bool HeldItemChangePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Slot;
	return true;
}
void HeldItemChangePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



DisplayScoreboardPacket::DisplayScoreboardPacket()
{}
bool DisplayScoreboardPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	uint8 pos;
	MCString name;

	data >> pos >> name;

	m_Position = (ScoreboardPosition)pos;
	m_Name = name.GetUTF16();

	return true;
}
void DisplayScoreboardPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



EntityMetadataPacket::EntityMetadataPacket() : CMinecraftInboundPacket(), m_Metadata(m_ProtocolVersion)
{

}
bool EntityMetadataPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	m_Metadata.SetProtocolVersion(m_ProtocolVersion);

	data >> eid;
	data >> m_Metadata;

	m_EntityId = eid.GetInt();

	return true;
}
void EntityMetadataPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



AttachEntityPacket::AttachEntityPacket()
{}
bool AttachEntityPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_EntityId;
	data >> m_VehicleId;
	return true;
}
void AttachEntityPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



EntityVelocityPacket::EntityVelocityPacket()
{}
bool EntityVelocityPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	data >> eid;
	data >> m_Velocity.x;
	data >> m_Velocity.y;
	data >> m_Velocity.z;
	m_EntityId = eid.GetInt();
	return true;
}
void EntityVelocityPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



EntityEquipmentPacket::EntityEquipmentPacket()
{}
bool EntityEquipmentPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;
	CVarInt equipmentSlot;

	data >> eid;
	data >> equipmentSlot;
	m_Item.Deserialize(data, m_ProtocolVersion);

	m_EntityId = eid.GetInt();
	m_EquipmentSlot = (EquipmentSlot)equipmentSlot.GetInt();

	return true;
}
void EntityEquipmentPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



SetExperiencePacket::SetExperiencePacket()
{}
bool SetExperiencePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt level, total;
	data >> m_ExperienceBar >> level >> total;
	m_Level = level.GetInt();
	m_TotalExperience = level.GetInt();
	return true;
}
void SetExperiencePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



UpdateHealthPacket::UpdateHealthPacket()
{}
bool UpdateHealthPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt food;
	data >> m_Health >> food >> m_Saturation;
	m_Food = food.GetInt();
	return true;
}
void UpdateHealthPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



ScoreboardObjectivePacket::ScoreboardObjectivePacket()
{}
bool ScoreboardObjectivePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString objective, value, type;
	uint8 mode;

	data >> objective >> mode >> value >> type;

	m_Objective = objective.GetUTF16();
	m_Mode = (Mode)mode;
	m_Value = value.GetUTF16();
	m_Type = type.GetUTF16();

	return true;
}
void ScoreboardObjectivePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



SetPassengersPacket::SetPassengersPacket()
{}
bool SetPassengersPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid, count;

	data >> eid >> count;
	m_EntityId = eid.GetInt();

	for (int32 i = 0; i < count.GetInt(); ++i)
	{
		CVarInt peid;

		data >> peid;
		m_Passengers.push_back(peid.GetInt());
	}

	return true;
}
void SetPassengersPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



TeamsPacket::TeamsPacket()
{}
bool TeamsPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString name;
	uint8 mode;

	data >> name >> mode;

	m_TeamName = name.GetUTF16();
	m_Mode = (Mode)mode;

	switch (m_Mode)
	{
		case Mode::Create:
		{
			MCString display, prefix, suffix, visbility, collision;
			CVarInt count;

			data >> display >> prefix >> suffix >> m_FriendlyFlags >> visbility >> collision >> m_Color >> count;

			m_TeamDisplayName = display.GetUTF16();
			m_TeamPrefix = prefix.GetUTF16();
			m_TeamSuffix = suffix.GetUTF16();
			m_TagVisbility = visbility.GetUTF16();
			m_CollisionRule = collision.GetUTF16();

			for (int32 i = 0; i < count.GetInt(); ++i)
			{
				MCString player;
				data >> player;

				m_Players.push_back(player.GetUTF16());
			}
		}
		break;
		case Mode::Remove:
		{

		}
		break;
		case Mode::Update:
		{
			MCString display, prefix, suffix, visbility, collision;

			data >> display >> prefix >> suffix >> m_FriendlyFlags >> visbility >> collision >> m_Color;

			m_TeamDisplayName = display.GetUTF16();
			m_TeamPrefix = prefix.GetUTF16();
			m_TeamSuffix = suffix.GetUTF16();
			m_TagVisbility = visbility.GetUTF16();
			m_CollisionRule = collision.GetUTF16();
		}
		break;
		case Mode::AddPlayer:
		case Mode::RemovePlayer:
		{
			CVarInt count;

			data >> count;

			for (int32 i = 0; i < count.GetInt(); ++i)
			{
				MCString player;
				data >> player;

				m_Players.push_back(player.GetUTF16());
			}
		}
		break;
	}

	return true;
}
void TeamsPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



UpdateScorePacket::UpdateScorePacket()
{}
bool UpdateScorePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString name, objective;
	uint8 action;

	data >> name >> action >> objective;

	m_ScoreName = name.GetUTF16();
	m_Action = (Action)action;
	m_Objective = objective.GetUTF16();

	if (m_Action != Action::Remove)
	{
		CVarInt value;
		data >> value;
		m_Value = value.GetInt();
	}
	return true;
}
void UpdateScorePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



SpawnPositionPacket::SpawnPositionPacket()
{}
bool SpawnPositionPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_Location;
	return true;
}
void SpawnPositionPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



TimeUpdatePacket::TimeUpdatePacket()
{}
bool TimeUpdatePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	data >> m_WorldAge;
	data >> m_Time;
	return true;
}
void TimeUpdatePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


TitlePacket::TitlePacket()
{}
bool TitlePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt action;
	data >> action;

	m_Action = (Action)action.GetInt();

	switch (m_Action)
	{
		case Action::SetTitle:
		case Action::SetSubtitle:
		case Action::SetActionBar:
		{
			MCString text;
			data >> text;

			m_Text = text.GetUTF16();
		}
		break;
		case Action::SetDisplay:
		{
			data >> m_FadeIn >> m_Stay >> m_FadeOut;
		}
		break;
		default:
			break;
	}

	return true;
}
void TitlePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


SoundEffectPacket::SoundEffectPacket()
{}
bool SoundEffectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt id, category;

	data >> id >> category;

	m_SoundId = id.GetInt();
	m_Category = (SoundCategory)category.GetInt();

	FixedPointNumber<int> x, y, z;
	data >> x >> y >> z;

	m_Position.x = (double)x.GetFloat();
	m_Position.y = (double)y.GetFloat();
	m_Position.z = (double)z.GetFloat();
	data >> m_Volume;
	data >> m_Pitch;

	return true;
}
void SoundEffectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



PlayerListHeaderAndFooterPacket::PlayerListHeaderAndFooterPacket()
{}
bool PlayerListHeaderAndFooterPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	MCString header, footer;

	data >> header >> footer;

	m_Header = header.GetUTF16();
	m_Footer = footer.GetUTF16();

	return true;
}
void PlayerListHeaderAndFooterPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



CollectItemPacket::CollectItemPacket()
{}
bool CollectItemPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt collected, collector, count;

	data >> collected >> collector >> count;

	m_Collected = collected.GetInt();
	m_Collector = collector.GetInt();
	m_PickupCount = count.GetInt();

	return true;
}
void CollectItemPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}



EntityTeleportPacket::EntityTeleportPacket()
{}
bool EntityTeleportPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid;

	m_EntityId = eid.GetInt();

	data >> m_Position.x >> m_Position.y >> m_Position.z;

	data >> m_Yaw >> m_Pitch;
	data >> m_OnGround;

	return true;
}
void EntityTeleportPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityPropertiesPacket::EntityPropertiesPacket()
{}
bool EntityPropertiesPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid;

	data >> eid;
	m_EntityId = eid.GetInt();

	int32 propertyCount;
	data >> propertyCount;

	for (int32 i = 0; i < propertyCount; ++i)
	{
		MCString key;
		double value;

		data >> key;
		data >> value;

		Attribute attribute(key.GetUTF16(), value);

		CVarInt modifierCount;
		data >> modifierCount;

		for (int32 j = 0; j < modifierCount.GetInt(); ++j)
		{
			CUUID uuid;
			double amount;
			uint8 operation;

			data >> uuid;
			data >> amount;
			data >> operation;

			Modifier modifier(uuid, amount, (ModifierOperation)operation);

			attribute.AddModifier(modifier);
		}

		m_Properties.insert(std::make_pair(attribute.GetKey(), attribute));
	}
	return true;
}
void EntityPropertiesPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


EntityEffectPacket::EntityEffectPacket()
{}
bool EntityEffectPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt eid, duration;
	data >> eid >> m_EffectId >> m_Amplifier >> duration >> m_Flags;
	m_EntityId = eid.GetInt();
	m_Duration = duration.GetInt();
	return true;
}
void EntityEffectPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


AdvancementProgressPacket::AdvancementProgressPacket()
{}
bool AdvancementProgressPacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	bool hasId;
	data >> hasId;

	if (hasId)
	{
		MCString id;

		data >> id;

		m_Id = id.GetUTF16();
	}

	return true;
}
void AdvancementProgressPacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}


CraftRecipeResponsePacket::CraftRecipeResponsePacket()
{}
bool CraftRecipeResponsePacket::Deserialize(DataBuffer& data, std::size_t packetLength)
{
	CVarInt recipeId;

	data >> m_WindowId >> recipeId;

	m_RecipeId = recipeId.GetInt();

	return true;
}
void CraftRecipeResponsePacket::Dispatch(PacketHandler* handler)
{
	handler->HandlePacket(this);
}

} // ns in
