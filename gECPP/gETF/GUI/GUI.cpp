//
// Created by scion on 3/1/2024.
//

#include "GUI.h"
#include <gETF/File.h>
#include <Engine/Entity/Entity.h>
#include <Demo/Engine/Entity/FlyCamera.h>

namespace gETF::UI
{
	void Element::Serialize(istream& ptr, const File& settings)
	{
		_window = nullptr;
	}

	void Element::Deserialize(ostream& buf, const File& settings) const
	{
	}

	void Frame::Serialize(istream& ptr, const File& settings)
	{
	}

	void Frame::Deserialize(ostream& buf, const File& settings) const
	{

	}
}