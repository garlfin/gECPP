//
// Created by scion on 9/27/2024.
//

#pragma once

#include <Engine/Math/Math.h>
#include <Engine/Utility/Manager.h>
#include <Graphics/Buffer/VAO.h>

namespace gE
{
	class Material;
	struct DrawCallManager;

	struct DrawCall final : public Managed<DrawCall>
	{
	public:
		DrawCall(Manager<DrawCall>& manager) : Managed(*this, &manager) {}

		API::VAO* VAO = nullptr;
		Material* Material = nullptr;
		u8 SubMesh = 0;
		u8 LOD = 0;

	private:

		Managed* _previousVAO = nullptr, * _nextVAO = nullptr;
		Managed* _previousMaterial = nullptr, * _nextMaterial = nullptr;
		Managed* _previousSubMesh = nullptr, * _nextSubMesh = nullptr;
		Managed* _previousLOD = nullptr, * _nextLOD = nullptr;
	};

	struct DrawCallManager : public Manager<DrawCall>
	{

	};
}
