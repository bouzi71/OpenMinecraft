#ifndef MCLIB_BLOCK_MONSTERSPAWNER_H_
#define MCLIB_BLOCK_MONSTERSPAWNER_H_

#include <block/BlockEntity.h>




class MonsterSpawner 
	: public BlockEntity 
{
public:
    struct SpawnPotential {
        std::wstring type;
        int32 weight;
    };
private:
    std::vector<SpawnPotential> m_SpawnPotentials;
    std::wstring m_EntityId;
    int16 m_SpawnCount;
    int16 m_SpawnRange;
    int16 m_Delay;
    int16 m_MinSpawnDelay;
    int16 m_MaxSpawnDelay;
    int16 m_MaxNearbyEntities;
    int16 m_RequiredPlayerRange;

public:
    MCLIB_API MonsterSpawner(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) { }
    MCLIB_API bool ImportNBT(NBT* nbt);

    inline const std::vector<SpawnPotential>& GetSpawnPotentials() const noexcept { return m_SpawnPotentials; }
    inline const std::wstring& GetEntityId() const noexcept { return m_EntityId; }
    inline int16 GetSpawnCount() const noexcept { return m_SpawnCount; }
    inline int16 GetSpawnRange() const noexcept { return m_SpawnRange; }
    inline int16 GetDelay() const noexcept { return m_Delay; }
    inline int16 GetMinSpawnDelay() const noexcept { return m_MinSpawnDelay; }
    inline int16 GetMaxSpawnDelay() const noexcept { return m_MaxSpawnDelay; }
    inline int16 GetMaxNearbyEntities() const noexcept { return m_MaxNearbyEntities; }
    inline int16 GetRequiredPlayerRange() const noexcept { return m_RequiredPlayerRange; }
};

#endif
