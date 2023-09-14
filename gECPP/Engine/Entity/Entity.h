//
// Created by scion on 8/26/2023.
//

#pragma once

#include "Prototype.h"
#include "Engine/Component/Transform.h"

namespace gE
{
	class Entity
	{
	 public:
		Entity(Window*, Entity* parent = nullptr, const char* = nullptr);

		NODISCARD ALWAYS_INLINE Window* GetWindow() const { return _window; }
		NODISCARD ALWAYS_INLINE Transform& GetTransform() { return _transform; }
		NODISCARD ALWAYS_INLINE const Transform& GetTransform() const { return _transform; }
		NODISCARD ALWAYS_INLINE const char* GetName() const { return _name; }
		NODISCARD ALWAYS_INLINE Entity* GetParent() { return _parent; }

		virtual void OnInit() {};
		virtual void OnDestroy() {};

		virtual ~Entity() { delete[] _name; }

	 private:
		Window* const _window;
		const char* _name;
		Transform _transform;
		Entity* const _parent;
	};
}