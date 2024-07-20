//
// Created by scion on 7/15/2024.
//

#include "Cullable.h"
#include <Engine/Entity/Entity.h>
#include <Engine/Window.h>

gE::Cullable::Cullable(Entity* owner, const AABB_T& bounds)
	: Component(owner, &owner->GetWindow().GetCullingManager()), _meshBounds(bounds)
{

}

void gE::Cullable::OnUpdate(float d)
{
	Transform& transform = GetOwner()->GetTransform();

	_transformedBounds = _meshBounds * transform.Model();
}

void gE::Cullable::OnRender(float d, Camera* camera)
{
	_flags.Visible = true;
}

