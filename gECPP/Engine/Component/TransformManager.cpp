//
// Created by scion on 10/29/2023.
//

#include "TransformManager.h"

namespace gE
{
	void TransformManager::OnUpdate(float delta)
	{
		std::vector<Entity*> stack;
		for(Transform* t: *this)
		{
			stack.push_back(t->GetOwner());

			while(!stack.empty())
			{
				Entity* back = stack.back();
				back->GetTransform().OnUpdate(delta);
				stack.pop_back();

				for(Entity* child: back->GetChildren())
					stack.push_back(child);
			}
		}
	}

	void TransformManager::Register(Transform* t)
	{
		if(!t->GetOwner()->GetParent()) Manager::Register(t);
	}
}