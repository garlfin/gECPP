//
// Created by scion on 6/30/2025.
//

#pragma once

#include <iostream>
#include <Core/Array.h>

template<class T, class S>
concept is_serializable_in = requires(T t, S s, std::istream& i)
{
    t.Deserialize(i, s);
};

template<class T>
concept is_serializable_out = requires(T t, std::ostream& o)
{
    t.Serialize(o);
};

template<class T, class S>
concept is_serializable = is_serializable_in<S, T> and is_serializable_out<T>;

template<class T> void Read(std::istream& in, u32 count, T* t);
template<class T> void Read(std::istream& in, T& t) { Read(in, 1, &t); }
template<class T> T Read(std::istream& in) { T t; Read(in, t); return t; }

template<class T> void Write(std::ostream& out, u32 count, const T* t);
template<class T> void Write(std::ostream& out, const T& t) { Write(out, 1, &t); }

template<typename UINT_T, class T> void Read(std::istream& in, u32 count, Array<T>* t);
template<typename UINT_T, class T> void Read(std::istream& in, Array<T>& t) { Read<UINT_T>(in, 1, &t); }

template<typename UINT_T, class T> void WriteArray(std::ostream& out, u32 count, const Array<T>* t);
template<typename UINT_T, class T> void WriteArray(std::ostream& out, const Array<T>& t) { WriteArray<UINT_T>(out, 1, &t); }

template<> void Read(std::istream& in, u32 count, std::string* t);
template<> void Write(std::ostream& out, u32 count, const std::string* t);

template<> void Write(std::ostream& out, u32 count, const std::string_view* t);

#include "IO.inl"