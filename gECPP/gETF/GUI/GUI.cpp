//
// Created by scion on 3/1/2024.
//

#include "GUI.h"
#include <gETF/File.h>
#include <Engine/Entity/Entity.h>
#include <Demo/Engine/Entity/FlyCamera.h>

namespace gETF::UI
{
	void IElement::Serialize(istream& ptr, const Header& settings)
	{
		_window = nullptr;
	}

	void IElement::Deserialize(ostream& buf) const
	{

	}

	void Frame::Serialize(istream& ptr, const Header& settings)
	{
	}

	void Frame::Deserialize(ostream& buf) const
	{

	}
}