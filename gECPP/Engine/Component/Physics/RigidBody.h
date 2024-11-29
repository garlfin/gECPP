//
// Created by scion on 10/30/2024.
//

#pragma once

#include "Physics.h"
#include "Collider.h"

#include <Engine/Component/Component.h>

namespace gE
{
	struct PhysicsMaterial
	{
		float Friction = 0.5;
		float Bounciness = 0.1;
		float Buoyancy = 0.5;
	};

	struct RigidBodySettings
	{
		float LinearDamping = 0.05f;
		float AngularDamping = 0.05f;
		float TerminalVelocity = 500.f;
		float MaxAngularVelocity = 0.25f * glm::pi<float>() * 60.f;
		float GravityFactor = 1.0;
		PhysicsMaterial Material = DEFAULT;
	};

	class RigidBody final : public PhysicsComponent
	{
	public:
		RigidBody(Entity* owner, const RigidBodySettings&, Collider&);

		GET_CONST(const Collider&, Collider, *_collider);
		GET_CONST(const PhysicsMaterial&, Material, Material);
		void SetMaterial(const PhysicsMaterial& material);

		void OnInit() override;
		void OnEarlyFixedUpdate(float d) override;
		void OnFixedUpdate(float d) override;
		void OnDestroy() override;

		~RigidBody() override;

	protected:
		NODISCARD ALWAYS_INLINE Collider& GetCollider() { return *_collider; }

		PhysicsMaterial Material;

	private:
		RigidBodySettings _settings;
		RelativePointer<Collider> _collider;
		px::Body* _body = nullptr;
		glm::vec3 _previousScale;
	};
}