#ifndef MCLIB_BLOCK_SKULL_H_
#define MCLIB_BLOCK_SKULL_H_

#include <block/BlockEntity.h>
#include <common/UUID.h>



enum class SkullType {
    Skeleton,
    WitherSkeleton,
    Zombie,
    Head,
    Creeper,
    Dragon
};

MCLIB_API std::string to_string(SkullType type);
MCLIB_API std::wstring to_wstring(SkullType type);

class Skull : public BlockEntity {
public:
    struct Texture {
        std::wstring signature;
        std::wstring value;
    };
private:
    SkullType m_Type;
    uint8 m_Rotation;
    CUUID m_OwnerUUID;
    std::wstring m_OwnerName;
    std::vector<Texture> m_Textures;

public:
    MCLIB_API Skull(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) { }
    MCLIB_API bool ImportNBT(NBT* nbt);

    inline SkullType GetType() const noexcept { return m_Type; }
    inline uint8 GetRotation() const noexcept { return m_Rotation; }
    inline CUUID GetOwnerUUID() const noexcept { return m_OwnerUUID; }
    inline const std::wstring& GetOwnerName() const noexcept { return m_OwnerName; }
    inline const std::vector<Texture>& GetTextures() const noexcept { return m_Textures; }
};

#endif
