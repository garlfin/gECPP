//
// Created by scion on 11/10/2023.
//

#include "Prototype.h"
#include <Engine/Manager.h>

namespace gE
{
	Updateable::Updateable(Manager* manager, const Flags& flags) : _manager(manager), _flags(flags)
	{
		if(manager) manager->Register(this);
	}

	Updateable::~Updateable()
	{
		if(_manager) _manager->Remove(this);
	}
}

