#ifndef MCLIB_BLOCK_BED_H_
#define MCLIB_BLOCK_BED_H_

#include <block/BlockEntity.h>



class Bed : public BlockEntity {
private:
    int32 m_Color;

public:
    MCLIB_API Bed(BlockEntityType type, glm::ivec3 position) : BlockEntity(type, position) { }
    MCLIB_API bool ImportNBT(NBT* nbt);

    inline int32 GetColor() const noexcept { return m_Color; }
};

#endif
