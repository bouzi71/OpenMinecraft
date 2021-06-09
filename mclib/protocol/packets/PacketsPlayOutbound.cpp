#include "stdafx.h"

#include <protocol/packets/PacketsPlayOutbound.h>

#include <core/Connection.h>
#include <protocol/PacketHandler.h>


namespace out
{
// Play packets

TeleportConfirmPacket::TeleportConfirmPacket(s32 teleportId) : m_TeleportId(teleportId)
{

}
DataBuffer TeleportConfirmPacket::Serialize() const
{
	DataBuffer buffer;
	CVarInt teleportId(m_TeleportId);

	buffer << m_Id;
	buffer << teleportId;

	return buffer;
}


PrepareCraftingGridPacket::PrepareCraftingGridPacket(u8 windowId, s16 actionNumber, const std::vector<Entry>& returnEntries, const std::vector<Entry>& prepareEntries)
	: m_WindowId(windowId),
	m_ActionNumber(actionNumber),
	m_ReturnEntries(returnEntries),
	m_PrepareEntries(prepareEntries)
{

}
DataBuffer PrepareCraftingGridPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_WindowId;
	buffer << m_ActionNumber;

	s16 returnSize = static_cast<s16>(m_ReturnEntries.size());

	buffer << returnSize;
	for (auto&& entry : m_ReturnEntries)
	{
		buffer << entry.item.Serialize(m_ProtocolVersion);
		buffer << entry.craftingSlot;
		buffer << entry.playerSlot;
	}

	s16 prepareSize = static_cast<s16>(m_PrepareEntries.size());

	buffer << prepareSize;
	for (auto&& entry : m_PrepareEntries)
	{
		buffer << entry.item.Serialize(m_ProtocolVersion);
		buffer << entry.craftingSlot;
		buffer << entry.playerSlot;
	}

	return buffer;
}


CraftRecipeRequestPacket::CraftRecipeRequestPacket(u8 windowId, s32 recipeId, bool makeAll)
	: m_WindowId(windowId),
	m_RecipeId(recipeId),
	m_MakeAll(makeAll)
{

}
DataBuffer CraftRecipeRequestPacket::Serialize() const
{
	CVarInt recipeId(m_RecipeId);
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_WindowId;
	buffer << recipeId;
	buffer << m_MakeAll;

	return buffer;
}


TabCompletePacket::TabCompletePacket(const std::wstring& text, bool assumeCommand)
	: m_Text(text), m_AssumeCommand(assumeCommand), m_HasPosition(false)
{

}
TabCompletePacket::TabCompletePacket(const std::wstring& text, bool assumeCommand, bool hasPosition, Position lookingAt)
	: m_Text(text), m_AssumeCommand(assumeCommand), m_HasPosition(hasPosition), m_LookingAt(lookingAt)
{

}


DataBuffer TabCompletePacket::Serialize() const
{
	DataBuffer buffer;
	MCString text(m_Text);

	buffer << m_Id << text;
	buffer << m_AssumeCommand << m_HasPosition;
	if (m_HasPosition)
		buffer << m_LookingAt;

	return buffer;
}


ChatPacket::ChatPacket(const std::wstring& message) : m_Message(message)
{

}
ChatPacket::ChatPacket(const std::string& message) : m_Message(message.begin(), message.end())
{

}
DataBuffer ChatPacket::Serialize() const
{
	MCString out(m_Message);
	DataBuffer buffer;

	buffer << m_Id;
	buffer << out;

	return buffer;
}


ClientStatusPacket::ClientStatusPacket(Action action) : m_Action(action)
{

}
DataBuffer ClientStatusPacket::Serialize() const
{
	CVarInt action(m_Action);
	DataBuffer buffer;

	buffer << m_Id;
	buffer << action;

	return buffer;
}


ClientSettingsPacket::ClientSettingsPacket(const std::wstring& locale, u8 viewDistance, ChatMode chatMode, bool chatColors, u8 skinFlags, MainHand hand)
	: m_Locale(locale), m_ViewDistance(viewDistance), m_ChatMode(chatMode),
	m_ChatColors(chatColors), m_SkinFlags(skinFlags), m_MainHand(hand)
{

}
DataBuffer ClientSettingsPacket::Serialize() const
{
	MCString locale(m_Locale);
	DataBuffer buffer;
	CVarInt chatMode((int)m_ChatMode);
	CVarInt hand((int)m_MainHand);

	buffer << m_Id;
	buffer << locale;
	buffer << m_ViewDistance;
	buffer << chatMode;
	buffer << m_ChatColors;
	buffer << m_SkinFlags;
	buffer << hand;

	return buffer;
}


ConfirmTransactionPacket::ConfirmTransactionPacket(u8 windowId, s16 action, bool accepted)
	: m_WindowId(windowId), m_Action(action), m_Accepted(accepted)
{

}
DataBuffer ConfirmTransactionPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_WindowId;
	buffer << m_Action;
	buffer << m_Accepted;

	return buffer;
}


EnchantItemPacket::EnchantItemPacket(u8 windowId, u8 enchantmentIndex)
{

}
DataBuffer EnchantItemPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id << m_WindowId << m_EnchantmentIndex;

	return buffer;
}


ClickWindowPacket::ClickWindowPacket(u8 windowId, u16 slotIndex, u8 button, u16 action, s32 mode, Slot clickedItem)
	: m_WindowId(windowId), m_SlotIndex(slotIndex), m_Button(button), m_Action(action), m_Mode(mode), m_ClickedItem(clickedItem)
{

}
DataBuffer ClickWindowPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_WindowId << m_SlotIndex << m_Button << m_Action;
	CVarInt mode(m_Mode);
	buffer << mode << m_ClickedItem.Serialize(m_ProtocolVersion);

	return buffer;
}


CloseWindowPacket::CloseWindowPacket(u8 windowId)
	: m_WindowId(windowId)
{

}
DataBuffer CloseWindowPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_WindowId;

	return buffer;
}


PluginMessagePacket::PluginMessagePacket(const std::wstring& channel, const std::string& data)
	: m_Channel(channel), m_Data(data)
{

}
DataBuffer PluginMessagePacket::Serialize() const
{
	DataBuffer buffer;
	MCString channel(m_Channel);

	buffer << m_Id << channel;
	buffer << m_Data;

	return buffer;
}


UseEntityPacket::UseEntityPacket(EntityId target, Action action, Hand hand, Vector3f position)
	: m_Target(target), m_Action(action), m_Hand(hand), m_Position(position)
{

}
DataBuffer UseEntityPacket::Serialize() const
{
	DataBuffer buffer;
	CVarInt target(m_Target);
	CVarInt type((int)m_Action);

	buffer << m_Id;
	buffer << target;
	buffer << type;

	if (m_Action == Action::InteractAt)
	{
		buffer << m_Position.x;
		buffer << m_Position.y;
		buffer << m_Position.z;
	}

	if (m_Action == Action::Interact || m_Action == Action::InteractAt)
	{
		CVarInt hand((int)m_Hand);
		buffer << hand;
	}

	return buffer;
}


KeepAlivePacket::KeepAlivePacket(s64 id) : m_KeepAliveId(id)
{

}
DataBuffer KeepAlivePacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id;
	buffer << m_KeepAliveId;
	return buffer;
}


PlayerPositionPacket::PlayerPositionPacket(Vector3d position, bool onGround)
	: m_Position(position), m_OnGround(onGround)
{

}
DataBuffer PlayerPositionPacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id;
	buffer << m_Position.x << m_Position.y << m_Position.z;
	buffer << m_OnGround;
	return buffer;
}


PlayerPositionAndLookPacket::PlayerPositionAndLookPacket(Vector3d position, float yaw, float pitch, bool onGround)
	: m_Position(position), m_Yaw(yaw), m_Pitch(pitch), m_OnGround(onGround)
{

}
DataBuffer PlayerPositionAndLookPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_Position.x << m_Position.y << m_Position.z;
	buffer << m_Yaw << m_Pitch;
	buffer << m_OnGround;

	return buffer;
}


PlayerLookPacket::PlayerLookPacket(float yaw, float pitch, bool onGround)
	: m_Yaw(yaw), m_Pitch(pitch), m_OnGround(onGround)
{

}
DataBuffer PlayerLookPacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id;
	buffer << m_Yaw << m_Pitch;
	buffer << m_OnGround;
	return buffer;
}


PlayerPacket::PlayerPacket(bool onGround)
	: m_OnGround(onGround)
{

}
DataBuffer PlayerPacket::Serialize() const
{
	DataBuffer buffer;
	buffer << m_Id;
	buffer << m_OnGround;
	return buffer;
}


VehicleMovePacket::VehicleMovePacket(Vector3d position, float yaw, float pitch)
	: m_Position(position), m_Yaw(yaw), m_Pitch(pitch)
{

}
DataBuffer VehicleMovePacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_Position.x << m_Position.y << m_Position.z;
	buffer << m_Yaw << m_Pitch;

	return buffer;
}

SteerBoatPacket::SteerBoatPacket(bool rightPaddle, bool leftPaddle)
	: m_RightPaddle(rightPaddle), m_LeftPaddle(leftPaddle)
{

}
DataBuffer SteerBoatPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_RightPaddle << m_LeftPaddle;

	return buffer;
}


PlayerAbilitiesPacket::PlayerAbilitiesPacket(bool isFlying)
	: m_IsFlying(isFlying)
{

}
DataBuffer PlayerAbilitiesPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;

	u8 flags = (u8)m_IsFlying << 1;
	float flyingSpeed = 0.0f;
	float walkingSpeed = 0.0f;

	buffer << flags << flyingSpeed << walkingSpeed;

	return buffer;
}


PlayerDiggingPacket::PlayerDiggingPacket(Status status, Vector3i position, Face face)
	: m_Status(status), m_Position(position), m_Face(face)
{

}
DataBuffer PlayerDiggingPacket::Serialize() const
{
	DataBuffer buffer;
	Position location((s32)m_Position.x, (s32)m_Position.y, (s32)m_Position.z);

	buffer << m_Id;
	buffer << (u8)m_Status;
	buffer << location;
	buffer << (u8)m_Face;

	return buffer;
}


EntityActionPacket::EntityActionPacket(EntityId eid, Action action, s32 actionData)
	: m_EntityId(eid), m_Action(action), m_ActionData(actionData)
{

}
DataBuffer EntityActionPacket::Serialize() const
{
	DataBuffer buffer;
	CVarInt eid(m_EntityId);
	CVarInt action((s32)m_Action);
	CVarInt actionData(m_ActionData);

	buffer << m_Id;
	buffer << eid << action << actionData;

	return buffer;
}


SteerVehiclePacket::SteerVehiclePacket(float sideways, float forward, u8 flags)
	: m_Sideways(sideways), m_Forward(forward), m_Flags(flags)
{

}
DataBuffer SteerVehiclePacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_Sideways << m_Forward << m_Flags;

	return buffer;
}


ResourcePackStatusPacket::ResourcePackStatusPacket(Result result)
	: m_Result(result)
{

}
DataBuffer ResourcePackStatusPacket::Serialize() const
{
	DataBuffer buffer;
	CVarInt result((int)m_Result);

	buffer << m_Id << result;

	return buffer;
}


HeldItemChangePacket::HeldItemChangePacket(u16 slot)
	: m_Slot(slot)
{

}
DataBuffer HeldItemChangePacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_Slot;

	return buffer;
}


CreativeInventoryActionPacket::CreativeInventoryActionPacket(s16 slot, Slot item)
	: m_Slot(slot),
	m_Item(item)
{

}
DataBuffer CreativeInventoryActionPacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id;
	buffer << m_Slot;
	buffer << m_Item.Serialize(m_ProtocolVersion);

	return buffer;
}


UpdateSignPacket::UpdateSignPacket(Vector3d position, const std::wstring& line1, const std::wstring& line2, const std::wstring& line3, const std::wstring& line4)
	: m_Line1(line1), m_Line2(line2), m_Line3(line3), m_Line4(line4)
{
	m_Position = Position((s32)position.x, (s32)position.y, (s32)position.z);
}
DataBuffer UpdateSignPacket::Serialize() const
{
	DataBuffer buffer;
	MCString line1(m_Line1);
	MCString line2(m_Line2);
	MCString line3(m_Line3);
	MCString line4(m_Line4);

	buffer << m_Id;
	buffer << m_Position;
	buffer << line1 << line2;
	buffer << line3 << line4;

	return buffer;
}


AnimationPacket::AnimationPacket(Hand hand)
	: m_Hand(hand)
{

}
DataBuffer AnimationPacket::Serialize() const
{
	DataBuffer buffer;
	CVarInt hand((int)m_Hand);

	buffer << m_Id << hand;

	return buffer;
}


SpectatePacket::SpectatePacket(CUUID uuid)
	: m_UUID(uuid)
{

}
DataBuffer SpectatePacket::Serialize() const
{
	DataBuffer buffer;

	buffer << m_Id << m_UUID;

	return buffer;
}


PlayerBlockPlacementPacket::PlayerBlockPlacementPacket(Vector3i position, Face face, Hand hand, Vector3f cursorPos)
	: m_Position(position), m_Face(face), m_Hand(hand), m_CursorPos(cursorPos)
{

}
DataBuffer PlayerBlockPlacementPacket::Serialize() const
{
	DataBuffer buffer;
	Position location((s32)m_Position.x, (s32)m_Position.y, (s32)m_Position.z);
	CVarInt face((u8)m_Face), hand((int)m_Hand);

	buffer << m_Id;
	buffer << location;
	buffer << face;
	buffer << hand;
	buffer << m_CursorPos.x;
	buffer << m_CursorPos.y;
	buffer << m_CursorPos.z;

	return buffer;
}


UseItemPacket::UseItemPacket(Hand hand)
	: m_Hand(hand)
{

}
DataBuffer UseItemPacket::Serialize() const
{
	DataBuffer buffer;
	CVarInt hand((int)m_Hand);

	buffer << m_Id << hand;

	return buffer;
}

} // ns out
