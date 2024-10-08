//
// Created by scion on 10/1/2024.
//

#pragma once

#include "DrawCall.h"

namespace gE
{
	inline DrawCall::DrawCall(DrawCallManager& manager, const MeshRenderer& r, Reference<Material>&& mat, u8 mesh) :
		Managed(&manager, *this),
		_renderer(&r), _material(move(mat)), _subMesh(mesh)
	{

	}

	inline void DrawCallManager::OnRegister(Managed<DrawCall>& t)
	{
		List.Add(t.GetIterator());
	}
}
