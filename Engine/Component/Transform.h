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
		REFLECTABLE_PROTO("gE::Transform", "TRFM", Transform, Component);

	public:
		Transform(Entity* o, const TransformData& d);

		explicit Transform(Entity* o);

		NODISCARD glm::mat4 GetParentTransform() const;

		void SetPosition(const glm::vec3& pos, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Position = pos; }
		void SetLocation(const glm::vec3& pos, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Position = pos; }
		void SetRotation(const glm::quat& rot, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Rotation = rot; }
		void SetScale(const glm::vec3& scale, TransformFlags flags = TransformFlags::All) { _flags |= flags; _transform.Scale = scale; }

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
		NODISCARD ALWAYS_INLINE const TransformData& operator*() const { return _transform; }

		GET_CONST(const TransformData&, GlobalTransform, _globalTransform);
		GET_CONST(TransformFlags, Flags, _flags);

		friend class TransformManager;
		friend class PhysicsComponent;

	protected:
		inline void Set(const TransformData& d, TransformFlags flags) { _flags |= flags; _transform = d; };
		inline void Set(const Transform& d, TransformFlags flags) { _flags |= flags; _transform = d._transform; };

		void SetPosition_(const glm::vec3& pos) { SetPosition(pos); }
		void SetRotation_(const glm::vec3& rot) { SetRotation(glm::radians(rot)); }
		void SetScale_(const glm::vec3& scale) { SetScale(scale); }

		GET_CONST(const glm::vec3&, Position_, _transform.Position);
		GET_CONST(glm::vec3, Rotation_, glm::degrees(glm::eulerAngles(_transform.Rotation)));
		GET_CONST(const glm::vec3&, Scale_, _transform.Scale);

	private:
		TransformData _transform, _globalTransform;
		glm::mat4 _model, _previousModel = glm::mat4(1.0);
		TransformFlags _flags;
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
