//
// Created by scion on 11/10/2023.
//

#include "Prototype.h"
#include <Engine/Manager.h>
#include <Engine/Component/Component.h>

namespace gE
{
	void IComponentManager::OnUpdate(float d)
	{
		for(Component* component : *this) component->OnUpdate(d);
	}

	void IComponentManager::OnRender(float d)
	{
		for(Component* component : *this) component->OnRender(d);
	}
}

