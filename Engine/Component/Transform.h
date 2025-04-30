//
// Created by scion on 8/25/2023.
//

#pragma once

#include <Core/Math/Math.h>
#include <Core/Math/Transform.h>

#include "Component.h"

namespace gE
{
	class TransformManager;

	enum class TransformFlags : u8
	{
		PhysicsInvalidated = 1, // Physics components must move the body
		RenderInvalidated = 1 << 1, // Model matrix must be updated
		None = 0,
		All = PhysicsInvalidated | RenderInvalidated
	};

	ENUM_OPERATOR(TransformFlags, |);
	ENUM_OPERATOR(TransformFlags, &);
	ENUM_OPERATOR_UNARY(TransformFlags, ~);

	class Transform final : public Component
	{
		REFLECTABLE_PROTO("TRFM", Transform, Component);

	public:
		Transform(Entity* o, const TransformData& d);

		explicit Transform(Entity* o);

		NODISCARD mat4 GetParentTransform() const;

		void SetPosition(const vec3& pos, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Position = pos; }
		void SetLocation(const vec3& pos, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Position = pos; }
		void SetRotation(const quat& rot, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Rotation = rot; }
		void SetScale(const vec3& scale, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Scale = scale; }

		ALWAYS_INLINE void Set(const TransformData& d, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform = d; }
		ALWAYS_INLINE void Set(const Transform& d, TransformFlags flags = TransformFlags::All) { Set(d._transform, flags); }

		void OnInit() override {};
		void OnUpdate(float) override;
		void OnRender(float, Camera*) override;
		void OnFixedUpdate(float) override;

		ALWAYS_INLINE operator const mat4&() const { return _model; }
		NODISCARD ALWAYS_INLINE const mat4& Model() const { return _model; }
		NODISCARD ALWAYS_INLINE const mat4& PreviousRenderModel() const { return _previousModel; }

		NODISCARD ALWAYS_INLINE const TransformData* operator->() const { return &_transform; }
		NODISCARD ALWAYS_INLINE const TransformData& operator*() const { return _transform; }

		GET_CONST(const TransformData&, GlobalTransform, _globalTransform);
		GET_CONST(TransformFlags, Flags, _flags);

		friend class TransformManager;
		friend class PhysicsComponent;

	protected:
		void SetPosition_(const vec3& pos) { SetPosition(pos); }
		void SetRotation_(const vec3& rot) { SetRotation(radians(rot)); }
		void SetScale_(const vec3& scale) { SetScale(scale); }

		GET_CONST(const vec3&, Position_, _transform.Position);
		GET_CONST(vec3, Rotation_, degrees(eulerAngles(_transform.Rotation)));
		GET_CONST(const vec3&, Scale_, _transform.Scale);

	private:
		TransformData _transform, _globalTransform;
		mat4 _model, _previousModel = mat4(1.0);
		TransformFlags _flags = DEFAULT;
	};

	class TransformManager final : public ComponentManager<Transform>
	{
	 public:
		using ComponentManager::ComponentManager;

		void OnUpdate(float delta) override;

	private:
		LinkedList<Managed<Component>> _deletionList = DEFAULT;
	};
}
