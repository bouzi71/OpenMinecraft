#ifndef MCLIB_ENTITY_ENTITY_H_
#define MCLIB_ENTITY_ENTITY_H_

#include <common/DataBuffer.h>
#include <common/Types.h>
#include <entity/Attribute.h>
#include <entity/Metadata.h>






enum class EntityType
{
	Item = 1,
	XPOrb,
	AreaEffectCloud,
	ElderGuardian,
	WitherSkeleton,
	Stray,
	ThrownEgg,
	LeashKnot,
	Painting,
	Arrow,
	Snowball,
	Fireball,
	SmallFireball,
	ThrownEnderpearl,
	EyeOfEnderSignal,
	ThrownPotion,
	ThrownExpBottle,
	ItemFrame,
	WitherSkull,
	PrimedTnt,
	FallingSand,
	FireworksRocketEntity,
	Husk,
	SpectralArrow,
	ShulkerBullet,
	DragonFireball,
	ZombieVillager,
	SkeletonHorse,
	ZombieHorse,
	ArmorStand,
	Donkey,
	Mule,
	EvocationFangs,
	EvocationIllager,
	Vex,
	VindicationIllager,
	IllusionIllager,

	MinecartCommandBlock = 40,
	Boat,
	MinecartRideable,
	MinecartChest,
	MinecartFurnace,
	MinecartTNT,
	MinecartHopper,
	MinecartSpawner,

	Creeper = 50,
	Skeleton,
	Spider,
	Giant,
	Zombie,
	Slime,
	Ghast,
	PigZombie,
	Enderman,
	CaveSpider,
	Silverfish,
	Blaze,
	LavaSlime,
	EnderDragon,
	WitherBoss,
	Bat,
	Witch,
	Endermite,
	Guardian,
	Shulker,

	Pig = 90,
	Sheep,
	Cow,
	Chicken,
	Squid,
	Wolf,
	Mooshroom,
	SnowMan,
	Ocelot,
	IronGolem,
	Horse,
	Rabbit,
	PolarBear,
	Llama,
	LlamaSpit,
	Parrot,

	Villager = 120,

	EnderCrystal = 200,

	// Not part of protocol
	Lightning = 251,
	FallingObject = 252,
	FishingHook = 253,
	Player = 254,
	Unknown
};

class Entity
{
public:
	using AttributeMap = std::unordered_map<std::wstring, Attribute>;

protected:
	AttributeMap m_Attributes;
	EntityMetadata m_Metadata;
	glm::dvec3 m_Position;
	glm::dvec3 m_Velocity;
	EntityId m_EntityId;
	// Stored in radians
	float m_Yaw, m_Pitch, m_HeadPitch;
	EntityId m_VehicleId;
	EntityType m_Type;

public:
	Entity(EntityId id, Version protocolVersion) noexcept : m_EntityId(id), m_VehicleId(-1), m_Type(EntityType::Unknown), m_Metadata(protocolVersion) {}
	virtual ~Entity() {}

	Entity(const Entity& rhs) = default;
	Entity& operator=(const Entity& rhs) = default;
	Entity(Entity&& rhs) = default;
	Entity& operator=(Entity&& rhs) = default;

	EntityId GetEntityId() const noexcept { return m_EntityId; }
	EntityId GetVehicleId() const noexcept { return m_VehicleId; }
	const glm::dvec3& GetPosition() const noexcept { return m_Position; }
	const glm::dvec3& GetVelocity() const noexcept { return m_Velocity; }
	float GetYaw() const noexcept { return m_Yaw; }
	float GetPitch() const noexcept { return m_Pitch; }
	float GetHeadPitch() const noexcept { return m_HeadPitch; }
	EntityType GetType() const noexcept { return m_Type; }
	const EntityMetadata& GetMetadata() const noexcept { return m_Metadata; }
	const AttributeMap& GetAttributes() const noexcept { return m_Attributes; }

	Attribute GetAttribute(const std::wstring& key)
	{
		auto iter = m_Attributes.find(key);
		if (iter == m_Attributes.end()) return Attribute(key, 0);
		return iter->second;
	}

	void SetPosition(const glm::dvec3& pos) noexcept { m_Position = pos; }
	void SetVelocity(const glm::dvec3& vel) noexcept { m_Velocity = vel; }
	void SetYaw(float yaw) noexcept { m_Yaw = yaw; }
	void SetPitch(float pitch) noexcept { m_Pitch = pitch; }
	void SetHeadPitch(float pitch) noexcept { m_HeadPitch = pitch; }
	void SetVehicleId(EntityId vid) noexcept { m_VehicleId = vid; }
	void SetType(EntityType type) { m_Type = type; }
	void SetMetadata(const EntityMetadata& metadata) { m_Metadata = metadata; }

	void SetAttribute(const std::wstring& key, const Attribute& attrib)
	{
		m_Attributes.erase(key);
		m_Attributes.insert(std::make_pair(key, attrib));
	}

	void ClearAttributes() { m_Attributes.clear(); }
};

typedef std::shared_ptr<Entity> EntityPtr;


#endif
