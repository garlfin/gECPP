//
// Created by scion on 3/1/2024.
//

#include "GUI.h"
#include <gETF/File.h>

namespace gETF::UI
{
	void Element::Serialize(u8*& ptr, const File& settings)
	{
		_window = nullptr;
		_transform = ::Read<Transform2D>(ptr);
	}

	void Element::Deserialize(SerializationBuffer& buf, const File& settings) const
	{
		buf.Push(_transform);
	}

	void Frame::Serialize(u8*& ptr, const File& settings)
	{

	}

	void Frame::Deserialize(SerializationBuffer& buf, const File& settings) const
	{

	}

	void Frame::OnRender(float d)
	{

	}

	void Frame::OnInteract(float d)
	{

	}
}