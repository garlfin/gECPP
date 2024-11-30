//
// Created by scion on 3/1/2024.
//

#include "GUI.h"

namespace gETF::UI
{
	void Element::ISerialize(istream& in, void* settings)
	{
		_window = nullptr;
	}

	void Element::IDeserialize(ostream& out) const
	{

	}

	void Frame::ISerialize(istream& in, void* s)
	{
	}

	void Frame::IDeserialize(ostream& out) const
	{

	}
}