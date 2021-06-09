#include "stdafx.h"

#include <common/VarInt.h>

#include <common/DataBuffer.h>

#include <ostream>


CVarInt::CVarInt() noexcept : m_Value(0)
{

}
CVarInt::CVarInt(s8 val) noexcept : m_Value(val)
{

}
CVarInt::CVarInt(s16 val) noexcept : m_Value(val)
{

}
CVarInt::CVarInt(s32 val) noexcept : m_Value(val)
{

}
CVarInt::CVarInt(s64 val) noexcept : m_Value(val)
{

}

std::size_t CVarInt::GetSerializedLength() const {
    DataBuffer buffer;
    buffer << *this;
    return buffer.GetSize();
}

DataBuffer& operator<<(DataBuffer& out, const CVarInt& var) {
    u64 uval = var.m_Value;

    int encoded = 0;
    char data[10];

    do {
        u8 nextByte = uval & 0x7F;
        uval >>= 7;
        if (uval)
            nextByte |= 0x80;
        data[encoded++] = nextByte;
    } while (uval);
    out << std::string(data, encoded);

    return out;
}

DataBuffer& operator>>(DataBuffer& in, CVarInt& var) {
    u64 value = 0;
    int shift = 0;

    if (in.IsFinished()) {
        var.m_Value = 0;
        return in;
    }

    std::size_t i = in.GetReadOffset();

    do {
        if (i >= in.GetSize())
            throw std::out_of_range("Failed reading CVarInt from DataBuffer.");
        value |= (u64)(in[i] & 0x7F) << shift;
        shift += 7;
    } while ((in[i++] & 0x80) != 0);

    in.SetReadOffset(i);

    var.m_Value = value;

    return in;
}

std::ostream& operator<<(std::ostream& out, const CVarInt& v) {
    return out << v.GetLong();
}
