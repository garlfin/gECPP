//
// Created by scion on 8/25/2023.
//

#pragma once

#include <Engine/Math/Math.h>

#include "Component.h"

namespace gE
{
	class TransformManager;

	enum class TransformFlags : u8
	{
		PhysicsInvalidated = 1,
		RenderInvalidated = 1 << 1,
		Initialized = 1 << 2,
		None = 0,
		All = PhysicsInvalidated | RenderInvalidated
	};

	ENUM_OPERATOR(TransformFlags, |);
	ENUM_OPERATOR(TransformFlags, &);
	ENUM_OPERATOR_LOGICAL(TransformFlags, ~);

	struct TransformData
	{
		union
		{
			glm::vec3 Location = glm::vec3(0.f);
			glm::vec3 Position;
		};
		glm::vec3 Scale = glm::vec3(1.f);
		glm::quaternion Rotation = glm::identity<glm::quat>();

		NODISCARD ALWAYS_INLINE glm::vec3 Forward() const { return Rotation * glm::vec3(0, 0, -1); }
		NODISCARD ALWAYS_INLINE glm::vec3 Up() const { return Rotation * glm::vec3(0, 1, 0); }
		NODISCARD ALWAYS_INLINE glm::vec3 Right() const { return Rotation * glm::vec3(1, 0, 0); }
		GET_CONST(glm::mat3, RotationMatrix, glm::toMat3(Rotation));

		static TransformData mix(const TransformData& a, const TransformData& b, float factor);

		glm::mat4 ToMat4() const;
	};

	class Transform final : public Component
	{
	 public:
		Transform(Entity* o, const TransformData& d);

		explicit Transform(Entity* o);

		NODISCARD glm::mat4 GetParentTransform() const;

		ALWAYS_INLINE void SetPosition(const glm::vec3& pos) { SetPosition(pos, TransformFlags::All); }
		ALWAYS_INLINE void SetLocation(const glm::vec3& pos) { SetLocation(pos, TransformFlags::All); }
		ALWAYS_INLINE void SetRotation(const glm::quat& rot) { SetRotation(rot, TransformFlags::All); }
		ALWAYS_INLINE void SetScale(const glm::vec3& scale) { SetScale(scale, TransformFlags::All); }

		ALWAYS_INLINE void Set(const TransformData& d) { Set(d, TransformFlags::All); }
		ALWAYS_INLINE void Set(const Transform& d) { Set(d._transform, TransformFlags::All); }

		void OnUpdate(float) override;
		void OnRender(float, Camera*) override;
		void OnFixedUpdate(float) override;

		ALWAYS_INLINE operator const glm::mat4&() const { return _model; }
		NODISCARD ALWAYS_INLINE const glm::mat4& Model() const { return _model; }
		NODISCARD ALWAYS_INLINE const glm::mat4& PreviousRenderModel() const { return _previousModel; }

		NODISCARD ALWAYS_INLINE const TransformData* operator->() const { return &_transform; }

		GET_CONST(const TransformData&, GlobalTransform, _globalTransform);

		friend class TransformManager;
		friend class RigidBody;

	protected:
		inline void SetPosition(const glm::vec3& pos, TransformFlags flags) { _flags |= flags; _transform.Position = pos; }
		inline void SetLocation(const glm::vec3& pos, TransformFlags flags) { _flags |= flags; _transform.Position = pos; }
		inline void SetRotation(const glm::quat& rot, TransformFlags flags) { _flags |= flags; _transform.Rotation = rot; }
		inline void SetScale(const glm::vec3& scale, TransformFlags flags) { _flags |= flags; _transform.Scale = scale; }

		inline void Set(const TransformData& d, TransformFlags flags) { _flags |= flags; _transform = d; };
		inline void Set(const Transform& d, TransformFlags flags) { _flags |= flags; _transform = d._transform; };

	private:
		TransformData _transform, _globalTransform;
		glm::mat4 _model, _previousModel = glm::mat4(1.0);
		TransformFlags _flags;
	};

	class TransformManager : public ComponentManager<Transform>
	{
	 public:
		using ComponentManager::ComponentManager;

		void OnUpdate(float delta) override;

	private:
		LinkedList<Managed<Component>> _deletionList = DEFAULT;
	};
}
