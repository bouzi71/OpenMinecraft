#pragma once

#include <protocol/packets/Packet.h>

class Connection;
class PacketHandler;


namespace out
{

class TeleportConfirmPacket 
	: public CMinecraftOutboundPacket
{ // 0x00
public:
	MCLIB_API TeleportConfirmPacket(s32 teleportId);

	DataBuffer MCLIB_API Serialize() const;

private:
	s32 m_TeleportId;
};


class PrepareCraftingGridPacket 
	: public CMinecraftOutboundPacket
{
public:
	struct Entry
	{
		Slot item;
		u8 craftingSlot;
		u8 playerSlot;
	};

public:
	MCLIB_API PrepareCraftingGridPacket(u8 windowId, s16 actionNumber, const std::vector<Entry>& returnEntries, const std::vector<Entry>& prepareEntries);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	u8 m_WindowId;
	s16 m_ActionNumber;
	std::vector<Entry> m_ReturnEntries;
	std::vector<Entry> m_PrepareEntries;
};


class CraftRecipeRequestPacket 
	: public CMinecraftOutboundPacket
{
public:
	MCLIB_API CraftRecipeRequestPacket(u8 windowId, s32 recipeId, bool makeAll);

	DataBuffer MCLIB_API Serialize() const;

private:
	u8 m_WindowId;
	s32 m_RecipeId;
	bool m_MakeAll;
};


class TabCompletePacket 
	: public CMinecraftOutboundPacket
{ // 0x01
public:
	MCLIB_API TabCompletePacket(const std::wstring& text, bool assumeCommand);
	MCLIB_API TabCompletePacket(const std::wstring& text, bool assumeCommand, bool hasPosition, Position lookingAt);

	DataBuffer MCLIB_API Serialize() const;

private:
	std::wstring m_Text;
	bool m_AssumeCommand;
	bool m_HasPosition;
	Position m_LookingAt;
};


class ChatPacket 
	: public CMinecraftOutboundPacket
{ // 0x02
public:
	MCLIB_API ChatPacket(const std::wstring& message);
	MCLIB_API ChatPacket(const std::string& message);

	DataBuffer MCLIB_API Serialize() const;

	const std::wstring& GetChatMessage() const { return m_Message; }

private:
	std::wstring m_Message;
};


class ClientStatusPacket 
	: public CMinecraftOutboundPacket
{ // 0x03
public:
	enum Action 
	{ 
		PerformRespawn, 
		RequestStats, 
		TakingInventoryAchievement 
	};

public:
	MCLIB_API ClientStatusPacket(Action action);

	DataBuffer MCLIB_API Serialize() const;

	Action GetAction() const { return m_Action; }

private:
	Action m_Action;
};


class ClientSettingsPacket 
	: public CMinecraftOutboundPacket
{ // 0x04
public:
	MCLIB_API ClientSettingsPacket(const std::wstring& locale, u8 viewDistance, ChatMode chatMode, bool chatColors, u8 skinFlags, MainHand hand);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	std::wstring m_Locale;
	u8 m_ViewDistance;
	ChatMode m_ChatMode;
	bool m_ChatColors;
	u8 m_SkinFlags;
	MainHand m_MainHand;
};


class ConfirmTransactionPacket 
	: public CMinecraftOutboundPacket
{ // 0x05
public:
	MCLIB_API ConfirmTransactionPacket(u8 windowId, s16 action, bool accepted);

	DataBuffer MCLIB_API Serialize() const;

private:
	u8 m_WindowId;
	s16 m_Action;
	bool m_Accepted;
};


class EnchantItemPacket 
	: public CMinecraftOutboundPacket
{ // 0x06
public:
	MCLIB_API EnchantItemPacket(u8 windowId, u8 enchantmentIndex);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	u8 m_WindowId;
	u8 m_EnchantmentIndex;
};


class ClickWindowPacket 
	: public CMinecraftOutboundPacket
{ // 0x07
public:
	MCLIB_API ClickWindowPacket(u8 windowId, u16 slotIndex, u8 button, u16 action, s32 mode, Slot clickedItem);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	u8 m_WindowId;
	u16 m_SlotIndex;
	u8 m_Button;
	u16 m_Action;
	s32 m_Mode;
	Slot m_ClickedItem;
};


class CloseWindowPacket 
	: public CMinecraftOutboundPacket
{ // 0x08
public:
	MCLIB_API CloseWindowPacket(u8 windowId);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	u8 m_WindowId;
};


class PluginMessagePacket 
	: public CMinecraftOutboundPacket
{ // 0x09
public:
	MCLIB_API PluginMessagePacket(const std::wstring& channel, const std::string& data);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	std::wstring m_Channel;
	std::string m_Data;
};


class UseEntityPacket 
	: public CMinecraftOutboundPacket
{ // 0x0A
public:
	enum class Action { Interact, Attack, InteractAt };

public:
	MCLIB_API UseEntityPacket(EntityId target, Action action, Hand hand = Hand::Main, Vector3f position = Vector3f(0, 0, 0));

	DataBuffer MCLIB_API Serialize() const;

private:
	EntityId m_Target;
	Action m_Action;
	Vector3f m_Position;
	Hand m_Hand;
};


class KeepAlivePacket 
	: public CMinecraftOutboundPacket
{ // 0x0B
public:
	MCLIB_API KeepAlivePacket(s64 id);

	DataBuffer MCLIB_API Serialize() const;

	s64 GetKeepAliveId() const { return m_KeepAliveId; }

private:
	s64 m_KeepAliveId;
};


class PlayerPositionPacket 
	: public CMinecraftOutboundPacket
{ // 0x0C
public:
	MCLIB_API PlayerPositionPacket(Vector3d position, bool onGround);

	DataBuffer MCLIB_API Serialize() const;

private:
	Vector3d m_Position;
	bool m_OnGround;
};


class PlayerPositionAndLookPacket 
	: public CMinecraftOutboundPacket
{ // 0x0D
public:
	MCLIB_API PlayerPositionAndLookPacket(Vector3d position, float yaw, float pitch, bool onGround);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	Vector3d m_Position;
	float m_Yaw;
	float m_Pitch;
	bool m_OnGround;
};


class PlayerLookPacket 
	: public CMinecraftOutboundPacket
{ // 0x0E
public:
	MCLIB_API PlayerLookPacket(float yaw, float pitch, bool onGround);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	float m_Yaw;
	float m_Pitch;
	bool m_OnGround;
};


class PlayerPacket 
	: public CMinecraftOutboundPacket
{ // 0x0F
public:
	MCLIB_API PlayerPacket(bool onGround);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	bool m_OnGround;
};


class VehicleMovePacket 
	: public CMinecraftOutboundPacket
{ // 0x10
public:
	MCLIB_API VehicleMovePacket(Vector3d position, float yaw, float pitch);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	Vector3d m_Position;
	float m_Yaw;
	float m_Pitch;
};


class SteerBoatPacket 
	: public CMinecraftOutboundPacket
{ // 0x11
public:
	MCLIB_API SteerBoatPacket(bool rightPaddle, bool leftPaddle);

	DataBuffer MCLIB_API Serialize() const;

private:
	bool m_RightPaddle;
	bool m_LeftPaddle;
};


class PlayerAbilitiesPacket 
	: public CMinecraftOutboundPacket
{ // 0x12
public:
	MCLIB_API PlayerAbilitiesPacket(bool isFlying);

	DataBuffer MCLIB_API Serialize() const;

private:
	bool m_IsFlying;
};


class PlayerDiggingPacket 
	: public CMinecraftOutboundPacket
{ // 0x13
public:
	enum Status
	{
		StartedDigging, 
		CancelledDigging, 
		FinishedDigging, 
		DropItemStack, 
		DropItem, 
		ShootArrow, 
		SwapHandItem
	};
public:
	MCLIB_API PlayerDiggingPacket(Status status, Vector3i position, Face face);

	DataBuffer MCLIB_API Serialize() const;

private:
	Status m_Status;
	Vector3i m_Position;
	Face m_Face;
};


class EntityActionPacket 
	: public CMinecraftOutboundPacket
{ // 0x14
public:
	enum class Action
	{
		StartSneak, 
		StopSneak,
		LeaveBed,
		StartSprint, 
		StopSprint,
		StartHorseJump, 
		StopHorseJump, 
		OpenRiddenHorseInventory,
		StartElytraFlying
	};

public:
	// Action data is only used for HorseJump (0 to 100), 0 otherwise.
	MCLIB_API EntityActionPacket(EntityId eid, Action action, s32 actionData = 0);
	DataBuffer MCLIB_API Serialize() const;

private:
	EntityId m_EntityId;
	Action m_Action;
	s32 m_ActionData;
};


class SteerVehiclePacket 
	: public CMinecraftOutboundPacket
{ // 0x15
public:
	// Flags: 0x01 = Jump, 0x02 = Unmount
	MCLIB_API SteerVehiclePacket(float sideways, float forward, u8 flags);
	DataBuffer MCLIB_API Serialize() const;

private:
	float m_Sideways;
	float m_Forward;
	u8 m_Flags;
};


class ResourcePackStatusPacket 
	: public CMinecraftOutboundPacket
{ // 0x16
public:
	enum class Result 
	{ 
		Loaded, 
		Declined, 
		Failed, 
		Accepted 
	};

public:
	MCLIB_API ResourcePackStatusPacket(Result result);
	DataBuffer MCLIB_API Serialize() const;

private:
	Result m_Result;
};


class CraftingBookDataPacket 
	: public CMinecraftOutboundPacket
{
public:
	MCLIB_API CraftingBookDataPacket();
	DataBuffer MCLIB_API Serialize() const;
};


class HeldItemChangePacket 
	: public CMinecraftOutboundPacket
{ // 0x17
public:
	// Slot should be between 0 and 8, representing hot bar left to right
	MCLIB_API HeldItemChangePacket(u16 slot);
	DataBuffer MCLIB_API Serialize() const;

private:
	u16 m_Slot;
};


class CreativeInventoryActionPacket 
	: public CMinecraftOutboundPacket
{ // 0x18
public:
	MCLIB_API CreativeInventoryActionPacket(s16 slot, Slot item);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	s16 m_Slot;
	Slot m_Item;
};


class UpdateSignPacket 
	: public CMinecraftOutboundPacket
{ // 0x19
public:
	MCLIB_API UpdateSignPacket(Vector3d position, const std::wstring& line1, const std::wstring& line2, const std::wstring& line3, const std::wstring& line4);
	
	DataBuffer MCLIB_API Serialize() const;

private:
	Position m_Position;
	std::wstring m_Line1;
	std::wstring m_Line2;
	std::wstring m_Line3;
	std::wstring m_Line4;
};



class AnimationPacket // Send when the player's arm swings
	: public CMinecraftOutboundPacket 
{ // 0x1A
public:
	MCLIB_API AnimationPacket(Hand hand = Hand::Main);

	DataBuffer MCLIB_API Serialize() const;

private:
	Hand m_Hand;
};


class SpectatePacket 
	: public CMinecraftOutboundPacket
{ // 0x1B
public:
	MCLIB_API SpectatePacket(CUUID uuid);

	DataBuffer MCLIB_API Serialize() const;

private:
	CUUID m_UUID;
};


class PlayerBlockPlacementPacket 
	: public CMinecraftOutboundPacket
{ // 0x1C
public:
	// Cursor position is the position of the crosshair on the block
	MCLIB_API PlayerBlockPlacementPacket(Vector3i position, Face face, Hand hand, Vector3f cursorPos);

	DataBuffer MCLIB_API Serialize() const;

private:
	Vector3i m_Position;
	Face m_Face;
	Hand m_Hand;
	Vector3f m_CursorPos;
};


class UseItemPacket 
	: public CMinecraftOutboundPacket
{ // 0x1D
public:
	MCLIB_API UseItemPacket(Hand hand);

	DataBuffer MCLIB_API Serialize() const;

private:
	Hand m_Hand;
};


class AdvancementTabPacket 
	: public CMinecraftOutboundPacket
{
private:

public:
	MCLIB_API AdvancementTabPacket();
	DataBuffer MCLIB_API Serialize() const;
};

} // ns out
