//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

gE::MeshRenderer::MeshRenderer(gE::Entity* o, const gETF::MeshReference& mesh, const gE::MaterialHolder* mat) :
	Component(o, &o->GetWindow().GetRenderers()),
	_mesh(mesh), _materialHolder(mat)
{
	if(_mesh->VAO) return;

	_mesh->CreateVAO(&GetWindow());
	_mesh->Free();
}

void gE::MeshRenderer::OnRender(float delta)
{

}

gE::Material& gE::MaterialHolder::GetMaterial(u8 i) const
{
	GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
	return _materials[i] || GetWindow().GetDefaultMaterial();
}