//
// Created by scion on 10/30/2024.
//

#pragma once

#include <Physics/Physics.h>
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
		float MaxAngularVelocity = 0.25f * glm::pi<float>() * 60.f;
		float GravityFactor = 1.0;
		PhysicsMaterial Material = DEFAULT;
	};

	enum class PhysicsInterpolationMode : u8
	{
		None,
		Interpolation,
		Extrapolation
	};

	class PhysicsComponent : public Component
	{
	public:
		using Component::Component;

		virtual void OnEarlyFixedUpdate(float d) = 0;

		GET_SET_VALUE(PhysicsInterpolationMode, InterpolationMode, _interpolationMode);

	private:
		PhysicsInterpolationMode _interpolationMode = PhysicsInterpolationMode::None;
	};

	class RigidBody : public PhysicsComponent
	{
	public:
		RigidBody(Entity* owner, const RigidBodySettings&, Collider&);

		GET_CONST(const Collider&, Collider, *_collider);
		GET_CONST(const PhysicsMaterial&, Material, Material);
		void SetMaterial(const PhysicsMaterial& material);

		void OnInit() override;
		void OnEarlyFixedUpdate(float d) override;
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

		GET(CollisionFilter&, Filter, _filter);
		GET(BroadPhaseFilter&, BroadFilter, _broadFilter);
		GET(px::PhysicsSystem&, System, _physics);
		GET(px::TempAllocator&, TempAllocator, _allocator);

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