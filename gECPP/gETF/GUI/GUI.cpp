//
// Created by scion on 3/1/2024.
//

#include "GUI.h"
#include <Demo/Engine/Entity/FlyCamera.h>

namespace gETF::UI
{
	void IElement::ISerialize(istream& in, const Header& settings)
	{
		_window = nullptr;
	}

	void IElement::IDeserialize(ostream& out) const
	{

	}

	void Frame::ISerialize(istream& in, const Header& s)
	{
	}

	void Frame::IDeserialize(ostream& out) const
	{

	}
}