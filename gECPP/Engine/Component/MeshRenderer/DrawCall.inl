//
// Created by scion on 10/1/2024.
//

#pragma once

#include "DrawCall.h"

namespace gE
{
	inline bool CompareVAO(const Managed<DrawCall>& a, const Managed<DrawCall>& b)
	{
		return &a->GetVAO() == &b->GetVAO();
	}

	inline bool CompareMaterial(const Managed<DrawCall>& a, const Managed<DrawCall>& b)
	{
		return a->GetMaterial() == b->GetMaterial();
	}

	inline bool CompareMaterialIndex(const Managed<DrawCall>& a, const Managed<DrawCall>& b)
	{
		return a->GetMaterialIndex() == b->GetMaterialIndex();
	}

	inline bool CompareLOD(const  Managed<DrawCall>& a, const Managed<DrawCall>& b)
	{
		return a->GetLOD() == b->GetLOD();
	}
}
