//
// Created by scion on 7/15/2024.
//

#pragma once

#include <Engine/Math/Collision/AABB.h>

#include "Component.h"

namespace gE
{
	struct CullFlags
	{
		bool Visible : 1 = true;
		bool Invalidated : 1 = false;
	};

	class Cullable : public Component
	{
	 public:
		typedef AABB<Dimension::D3D> AABB_T;

		Cullable(Entity* owner, const AABB_T& bounds);

		void OnUpdate(float d) override;
		void OnRender(float d, Camera* camera) override;

		GET_SET(AABB_T&, Bounds, _meshBounds);
		GET_CONST(const AABB_T&, GlobalBounds, _transformedBounds);
		GET_CONST(CullFlags, Flags, _flags);

		virtual ~Cullable() = default;

	 private:
		AABB_T _meshBounds;
		AABB_T _transformedBounds = DEFAULT;
		CullFlags _flags;
	};

	class CullingManager : public ComponentManager<Cullable>
	{
	 public:
	 private:
	};
}
