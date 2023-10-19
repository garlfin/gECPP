//
// Created by scion on 8/26/2023.
//

#pragma once

#include "GL/Binary.h"
#include "Prototype.h"
#include "Engine/Component/Transform.h"

namespace gE
{
	class Entity
	{
	 public:
		explicit Entity(Window*, Entity* parent = nullptr);

		ALWAYS_INLINE void Destroy() { _flags.Deletion = true; }
		GET_CONST_VALUE(Window*, Window, _window);
		GET(Transform&, Transform, _transform);
		GET_CONST_VALUE(Entity*, Parent, _parent);
		GET_CONST_VALUE(Flags, Flags, _flags);

		virtual void OnInit() {};
		virtual void OnUpdate(float delta) {};
		virtual void OnRender(float delta) {};
		virtual void OnDestroy() {};

		virtual ~Entity();

	 private:
		Window* const _window;
		Transform _transform;
		Entity* const _parent;
		Flags _flags;
	};
}