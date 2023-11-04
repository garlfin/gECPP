//
// Created by scion on 8/25/2023.
//

#pragma once

#include "GL/Math.h"
#include "Component.h"

namespace gE
{
	struct TransformData
	{
		union
		{
			glm::vec3 Location = glm::vec3(0.f);
			glm::vec3 Position;
		};
		glm::vec3 Scale = glm::vec3(1.f);
		glm::quaternion Rotation = glm::quaternion();
	};

	class Transform : public Component, public TransformData
	{
	 public:
		Transform(Entity* o, const TransformData& d) : Component(o)
		{ Set(d); }

		explicit Transform(Entity* o);

		glm::mat4 GetParentTransform();

		ALWAYS_INLINE void SetRotation(const glm::vec3& r) { Rotation = glm::quaternion(r); }

		void Set(const TransformData& d);
		void Set(const Transform& d);

		void OnUpdate(float) override { };
		void OnRender(float) override;

		NODISCARD ALWAYS_INLINE const glm::mat4& Model() const { return _model; }
		NODISCARD ALWAYS_INLINE glm::vec3 Forward() const { return Rotation * glm::vec3(0, 0, -1); }
		NODISCARD ALWAYS_INLINE glm::vec3 Up() const { return Rotation * glm::vec3(0, 1, 0); }
		NODISCARD ALWAYS_INLINE glm::vec3 Right() const { return Rotation * glm::vec3(1, 0, 0); }
		NODISCARD ALWAYS_INLINE glm::mat3 LocalRotationMatrix() const { return glm::toMat3(Rotation); }
		NODISCARD ALWAYS_INLINE glm::vec3 GlobalTranslation() const { return (glm::vec3) _model[3]; }

		~Transform() override;

	 private:
		glm::mat4 _model;
	};
}
