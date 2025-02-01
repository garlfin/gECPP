//
// Created by scion on 3/1/2024.
//

#include "GUI.h"

namespace gETF::UI
{
	void Element::IDeserialize(istream& in, SETTINGS_T s)
	{
		_window = nullptr;
	}

	void Element::ISerialize(ostream& out) const
	{

	}

	void Frame::IDeserialize(istream& in, SETTINGS_T s)
	{
	}

	void Frame::ISerialize(ostream& out) const
	{

	}
}