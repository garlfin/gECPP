//
// Created by scion on 10/30/2024.
//

#pragma once

#include "Physics.h"
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include <Engine/Component/Component.h>
#include <Engine/Utility/AssetManager.h>
#include "Collider.h"

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
		float MaxAngularVelocity = 0.25f * std::numbers::pi * 60.f;
		float GravityFactor = 1.0;
		PhysicsMaterial Material = DEFAULT;
	};

	class RigidBody : public Component
	{
	public:
		RigidBody(Entity* owner, const RigidBodySettings&, Collider&);

		GET_CONST(const Collider&, Collider, *_collider);
		GET_CONST(const PhysicsMaterial&, Material, Material);
		void SetMaterial(const PhysicsMaterial& material);

		void OnInit() override;
		void OnEarlyFixedUpdate(float d);
		void OnFixedUpdate(float d) override;
		void OnUpdate(float d) override;
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

	class PhysicsManager final : public ComponentManager<Component>
	{
	public:
		explicit PhysicsManager(Window* window);

		void OnFixedUpdate(float d) override;
		void OnEarlyFixedUpdate(float d);

		friend class RigidBody;

		~PhysicsManager() override;

	private:
		CollisionFilter _filter;
		BroadPhaseFilter _broadFilter;
		BroadPhase _broadPhase;

		SmartPointer<px::Factory> _factory;
		SmartPointer<px::TempAllocatorImpl> _allocator;
		SmartPointer<px::PhysicsSystem> _physics;
		SmartPointer<px::JobSystemThreadPool> _jobSystem;
		px::BodyInterface* _interface;
	};
}