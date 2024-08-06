//
// Created by scion on 3/1/2024.
//

#include "GUI.h"
#include <gETF/File.h>
#include <Demo/Engine/Entity/FlyCamera.h>

namespace gETF::UI
{
	void IElement::ISerialize(istream& ptr, const Header& settings)
	{
		_window = nullptr;
	}

	void IElement::IDeserialize(ostream& buf) const
	{

	}

	void Frame::ISerialize(istream& ptr, const Header& settings)
	{
	}

	void Frame::IDeserialize(ostream& buf) const
	{

	}
}