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
		Entity(Window*, Entity* parent = nullptr, const char* = nullptr);

		NODISCARD ALWAYS_INLINE Window* GetWindow() const { return _window; }
		NODISCARD ALWAYS_INLINE Transform& GetTransform() { return _transform; }
		NODISCARD ALWAYS_INLINE const Transform& GetTransform() const { return _transform; }
		NODISCARD ALWAYS_INLINE const char* GetName() const { return _name; }
		NODISCARD ALWAYS_INLINE Entity* GetParent() { return _parent; }

		virtual void OnInit() {};
		virtual void OnUpdate(float delta) {  }
		virtual void OnRender(float delta) { _transform.OnRender(delta); }
		virtual void OnDestroy() {};

		virtual ~Entity() { delete[] _name; }
	 protected:
		NODISCARD ALWAYS_INLINE gl::mat4& GetModel() { return _transform._model; }

	 private:
		Window* const _window;
		const char* _name;
		Transform _transform;
		Entity* const _parent;
	};
}