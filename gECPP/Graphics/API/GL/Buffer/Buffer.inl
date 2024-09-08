//
// Created by scion on 9/7/2024.
//

#pragma once

#include "Buffer.h"

namespace GL
{
	template<typename T, bool DYNAMIC>
	Buffer<T, DYNAMIC>::Buffer(gE::Window* window, const GPU::Buffer<T>& settings) :
		APIObject(window)
	{
		static constexpr size_t SIZE_T = sizeof(std::conditional_t<std::is_same_v<T, void>, uint8_t, T>);

		glCreateBuffers(1, &ID);

		if constexpr(DYNAMIC)
			glNamedBufferData(ID, SIZE_T * settings.GetCount(), settings.GetData(), GL_DYNAMIC_DRAW);
		else
			glNamedBufferStorage(ID, SIZE_T * settings.GetCount(), settings.GetData(), GL_DYNAMIC_STORAGE_BIT);
	}

	template<typename T, bool DYNAMIC>
	Buffer<T, DYNAMIC>::Buffer(gE::Window* window, u32 count, const T* data) :
		APIObject(window)
	{
		static constexpr size_t SIZE_T = sizeof(typename Array<T>::I);

		glCreateBuffers(1, &ID);

		if constexpr(DYNAMIC) glNamedBufferData(ID, SIZE_T * count, data, GL_DYNAMIC_DRAW);
		else glNamedBufferStorage(ID, SIZE_T * count, data, GL_DYNAMIC_STORAGE_BIT);
	}

	template<typename T, bool DYNAMIC>
	Buffer<T, DYNAMIC>::Buffer(gE::Window* window, const Array<T>& arr) :
		Buffer(window, arr.Count(), arr.Data())
	{
	}
}
