//
// Created by scion on 9/27/2024.
//

#pragma once

#include "MeshRenderer.h"

namespace gE
{
	inline bool MeshRenderer::CompareMaterial(const MeshRenderer& a, const MeshRenderer& b)
	{
		return a._materialHolder.
	}

	inline bool MeshRenderer::CompareLOD(const MeshRenderer& a, const MeshRenderer& b)
	{
		return true;
	}

	inline bool MeshRenderer::CompareVAO(const MeshRenderer& a, const MeshRenderer& b)
	{
		return &a.GetMesh() == &b.GetMesh();
	}
}