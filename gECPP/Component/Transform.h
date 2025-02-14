//
// Created by scion on 8/25/2023.
//

#pragma once

#include <Math/Math.h>
#include <Math/Transform.h>

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

	REFLECTABLE_BEGIN(Transform);
	class Transform final : public Component
	{
		REFLECTABLE_PROTO(Transform);

	 public:
		Transform(Entity* o, const TransformData& d);

		explicit Transform(Entity* o);

		NODISCARD glm::mat4 GetParentTransform() const;

		inline void SetPosition(const glm::vec3& pos, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Position = pos; }
		inline void SetLocation(const glm::vec3& pos, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Position = pos; }
		inline void SetRotation(const glm::quat& rot, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Rotation = rot; }
		inline void SetScale(const glm::vec3& scale, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Scale = scale; }

		ALWAYS_INLINE void Set(const TransformData& d) { Set(d, TransformFlags::All); }
		ALWAYS_INLINE void Set(const Transform& d) { Set(d._transform, TransformFlags::All); }

		void OnInit() override {};
		void OnUpdate(float) override;
		void OnRender(float, Camera*) override;
		void OnFixedUpdate(float) override;

		ALWAYS_INLINE operator const glm::mat4&() const { return _model; }
		NODISCARD ALWAYS_INLINE const glm::mat4& Model() const { return _model; }
		NODISCARD ALWAYS_INLINE const glm::mat4& PreviousRenderModel() const { return _previousModel; }

		NODISCARD ALWAYS_INLINE const TransformData* operator->() const { return &_transform; }

		GET_CONST(const TransformData&, GlobalTransform, _globalTransform);
		GET_CONST(TransformFlags, Flags, _flags);

		friend class TransformManager;
		friend class PhysicsComponent;

	protected:
		inline void Set(const TransformData& d, TransformFlags flags) { _flags |= flags; _transform = d; };
		inline void Set(const Transform& d, TransformFlags flags) { _flags |= flags; _transform = d._transform; };

	private:
		TransformData _transform, _globalTransform;
		glm::mat4 _model, _previousModel = glm::mat4(1.0);
		TransformFlags _flags;
	};
	inline REFLECTABLE_END(Transform, Component, "gE::Transform");
	inline REFLECTABLE_FACTORY_NO_IMPL(Transform);

	class TransformManager : public ComponentManager<Transform>
	{
	 public:
		using ComponentManager::ComponentManager;

		void OnUpdate(float delta) override;

	private:
		LinkedList<Managed<Component>> _deletionList = DEFAULT;
	};
}
