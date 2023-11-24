//
// Created by scion on 11/23/2023.
//

#include "Buffer.h"

namespace GL
{
	Buffer<void>* CreateBuffer(gE::Window* window, const BufferSettings& settings)
	{
		return new Buffer<void>(window, settings.Length, settings.Data);
	}

	DynamicBuffer<void>* CreateDynamicBuffer(gE::Window* window, const BufferSettings& settings)
	{
		return new DynamicBuffer<void>(window, settings.Length, settings.Data);
	}
}