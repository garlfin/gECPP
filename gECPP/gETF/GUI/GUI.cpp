//
// Created by scion on 3/1/2024.
//

#include "GUI.h"
#include <gETF/File.h>

namespace gETF::UI
{
	void Element::Serialize(istream& ptr, const File& settings)
	{
		_window = nullptr;
		_transform = ::Read<Transform>(ptr);
	}

	void Element::Deserialize(ostream& buf, const File& settings) const
	{
		Write(buf, _transform);
	}

	void Frame::Serialize(istream& ptr, const File& settings)
	{

	}

	void Frame::Deserialize(ostream& buf, const File& settings) const
	{

	}

	void Frame::OnRender(float d)
	{

	}

	void Frame::OnInteract(float d)
	{

	}
}