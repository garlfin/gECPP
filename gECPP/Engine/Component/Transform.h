//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Engine/Math/Math.h"
#include <Engine/Manager.h>
#include "Component.h"

namespace gE
{
	class TransformManager;

	struct TransformFlags
	{
		bool Invalidated : 1 = false;
		bool PreviousInvalidated : 1 = false;
		bool Initialized : 1 = false;
	};

	struct TransformData
	{
		union
		{
			glm::vec3 Location = glm::vec3(0.f);
			glm::vec3 Position;
		};
		glm::vec3 Scale = glm::vec3(1.f);
		glm::quaternion Rotation = glm::quaternion();

		NODISCARD ALWAYS_INLINE glm::vec3 Forward() const { return Rotation * glm::vec3(0, 0, -1); }
		NODISCARD ALWAYS_INLINE glm::vec3 Up() const { return Rotation * glm::vec3(0, 1, 0); }
		NODISCARD ALWAYS_INLINE glm::vec3 Right() const { return Rotation * glm::vec3(1, 0, 0); }
		GET_CONST(glm::mat3, RotationMatrix, glm::toMat3(Rotation));
	};

	class Transform : public Component
	{
	 public:
		Transform(Entity* o, const TransformData& d);

		explicit Transform(Entity* o);

		NODISCARD glm::mat4 GetParentTransform() const;

		ALWAYS_INLINE glm::vec3& SetPosition() { return SetLocation(); }
		inline glm::vec3& SetLocation() { _flags.Invalidated = true; return _transform.Location; }
		inline glm::quat& SetRotation() { _flags.Invalidated = true; return _transform.Rotation; }
		inline glm::vec3& SetScale() { _flags.Invalidated = true; return _transform.Scale; }

		inline void Set(const TransformData& d) { _flags.Invalidated = true; _transform = d; };
		inline void Set(const Transform& d) { _flags.Invalidated = true; _transform = d._transform; };

		void OnUpdate(float) override;
		void OnRender(float, Camera*) override;

		ALWAYS_INLINE operator const glm::mat4&() const { return _model; }
		NODISCARD ALWAYS_INLINE const glm::mat4& Model() const { return _model; }
		NODISCARD ALWAYS_INLINE const glm::mat4& PreviousModel() const { return _previousModel; }

		NODISCARD ALWAYS_INLINE const TransformData* operator->() const { return &_transform; }

		GET_CONST(const TransformData&, GlobalTransform, _globalTransform);

		friend class TransformManager;

	 private:
		TransformData _transform, _globalTransform;
		glm::mat4 _model, _previousModel = glm::mat4(1.0);
		TransformFlags _flags;
	};

	class TransformManager : public ComponentManager<Transform>
	{
	 public:
		using ComponentManager<Transform>::ComponentManager;

		void OnUpdate(float d) override;
	};
}
