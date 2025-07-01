//
// Created by scion on 6/30/2025.
//

#pragma once

#include "IO.h"

template<>
inline void Read(std::istream& in, u32 count, std::string* t)
{
    for(u32 i = 0; i < count; i++)
    {
        std::string& str = t[i];

        u32 length = Read<u32>(in);

        str = std::string(length, 0);
        Read(in, length, str.data());
    }
}


template<>
inline void Write(std::ostream& out, u32 count, const std::string* t)
{
    for(u32 i = 0; i < count; i++)
    {
        const std::string& str = t[i];

        u32 length = str.length();

        Write(out, length);
        Write(out, length, str.c_str());
    }
}

template<>
inline void Write(std::ostream& out, u32 count, const std::string_view* t)
{
    for(u32 i = 0; i < count; i++)
    {
        const std::string_view& str = t[i];

        u32 length = str.length();

        Write(out, length);
        Write(out, length, str.data());
    }
}

template<typename UINT_T, class T>
void Read(std::istream& in, u32 count, Array<T>* t)
{
    for(u32 i = 0; i < count; i++)
    {
        Array<T>& arr = t[i];

        UINT_T length = Read<UINT_T>(in);

        arr = Array<T>(length);
        Read<T>(in, length, arr.Data());
    }
}

template<typename UINT_T, class T>
void WriteArray(std::ostream& out, u32 count, const Array<T>* t)
{
    for(u32 i = 0; i < count; i++)
    {
        const Array<T>& arr = t[i];

        UINT_T length = arr.Size();

        Write(out, length);
        Write(out, length, arr.Data());
    }
}

template<typename UINT_T, class T>
void ReadArray(std::istream& in, Array<T>& array)
{
    static_assert(std::is_trivially_copyable_v<T>);

    UINT_T length = Read<UINT_T>(in);
    array = Array<T>(length);
    in.read((char*) array.Data(), length * sizeof(T));
}

template<typename UINT_T, class T>
Array<T> ReadArray(std::istream& in)
{
    Array<T> array;
    ReadArray<UINT_T, T>(in, array);
    return array;
}

template<typename T>
void Read(std::istream& in, u32 count, T* t)
{
    if(!count) return;
    static_assert(std::is_trivially_copyable_v<T> && !is_serializable_out<T>);
    in.read((char*) t, sizeof(T) * count);
}

template<typename T>
void Write(std::ostream& out, u32 count, const T* t)
{
    if(!count) return;
    if constexpr(std::is_trivially_copyable_v<T> && !is_serializable_out<T>)
        out.write((char*) t, sizeof(T) * count);
    else
        for(u32 i = 0; i < count; i++) t[i].Serialize(out);
}