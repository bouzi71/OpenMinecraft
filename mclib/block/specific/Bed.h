#ifndef MCLIB_BLOCK_BED_H_
#define MCLIB_BLOCK_BED_H_

#include <block/BlockEntity.h>



class Bed : public BlockEntity {
private:
    s32 m_Color;

public:
    MCLIB_API Bed(BlockEntityType type, Vector3i position) : BlockEntity(type, position) { }
    MCLIB_API bool ImportNBT(NBT* nbt);

    inline s32 GetColor() const noexcept { return m_Color; }
};

#endif
