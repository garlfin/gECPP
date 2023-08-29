//
// Created by scion on 8/26/2023.
//

#pragma once

#include "Forward.h"
#include "Components/Transform.h"

namespace gE
{
	class Entity
	{
	 public:
		Entity(Window* w, Entity* o = nullptr, const char* n = nullptr);

		NODISCARD ALWAYS_INLINE Window* GetWindow() const { return _window; }
		NODISCARD ALWAYS_INLINE Transform& GetTransform() { return _transform; }
		NODISCARD ALWAYS_INLINE const char* GetName() const { return _name; }
		NODISCARD ALWAYS_INLINE Entity* GetParent() { return _parent; }

		~Entity() { delete[] _name; }

	 private:
		Window* const _window;
		const char* _name;
		Transform _transform;
		Entity* const _parent;
	};
}