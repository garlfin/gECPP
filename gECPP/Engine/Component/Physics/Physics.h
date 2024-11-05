//
// Created by scion on 10/30/2024.
//

#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <Engine/Component/Component.h>
#include <Engine/Utility/AssetManager.h>

#define GE_PX_ALLOCATION 10485760
#define GE_PX_MAX_BODIES 1024
#define GE_PX_MAX_CONSTRAINTS (GE_PX_MAX_BODIES * 4)
#define GE_PX_MAX_BODY_PAIRS (GE_PX_MAX_BODIES * 4)
#define GE_PX_MIN_TICKRATE 60
#define GE_PX_MAX_STEPS 4

namespace px = JPH;

namespace gE
{
	class CollisionFilter final : public px::ObjectLayerPairFilter
	{
	public:
		NODISCARD inline bool ShouldCollide(px::ObjectLayer a, px::ObjectLayer b) const override;
	};

	class BroadPhaseFilter final : public px::ObjectVsBroadPhaseLayerFilter
	{
	public:
		NODISCARD inline bool ShouldCollide(px::ObjectLayer a, px::BroadPhaseLayer b) const override;
	};

	class BroadPhase final : public px::BroadPhaseLayerInterface
	{
	public:
		BroadPhase();

		NODISCARD inline px::uint GetNumBroadPhaseLayers() const override;
		NODISCARD inline px::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

	private:
		px::BroadPhaseLayer _layers[2] = DEFAULT;
	};

	struct PhysicsMaterial
	{
		float Friction = 0.5;
		float Bounciness = 0.1;
		float Buoyancy = 0.5;
	};

	struct RigidBodySettings
	{
		float Mass = 1.0;
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
		RigidBody(Entity* owner, const RigidBodySettings&, px::Shape&);

		GET_CONST(const px::Shape&, Shape, *_shape);
		GET_CONST(const PhysicsMaterial&, Material, Material);
		void SetMaterial(const PhysicsMaterial& material);

		void OnInit() override;
		void OnFixedUpdate(float d) override;
		void OnUpdate(float d) override;
		void OnRender(float d, Camera* camera) override;

		~RigidBody() override;

	protected:
		NODISCARD ALWAYS_INLINE px::Shape& GetShape() { return *_shape; }

		void FinalizeConstruction();

		PhysicsMaterial Material;

	private:
		RigidBodySettings _settings;
		px::Shape* _shape;
		px::Body* _body = nullptr;
	};

	class PhysicsManager final : public ComponentManager<Component>
	{
	public:
		explicit PhysicsManager(Window* window);

		void OnUpdate(float d) override;

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

	template<class T>
	struct ManagedPX
	{
	public:
		template<typename... ARGS>
		explicit ManagedPX(ARGS&&... args) : _t(std::forward<ARGS>(args)...)
		{
			_t.SetEmbedded();
		}

		GET(T&,, _t);

		ALWAYS_INLINE T* operator->() { return &_t; }
		ALWAYS_INLINE const T* operator->() const { return &_t; }
		ALWAYS_INLINE T& operator*() { return _t; }
		ALWAYS_INLINE const T& operator*() const { return _t; }

		DELETE_OPERATOR_CM(ManagedPX);

	private:
		T _t;
	};

	inline px::Vec3 ToPX(const glm::vec3& o)
	{
		return  { o.x, o.y, o.z };
	}

	inline glm::vec3 ToGLM(const px::Vec3& o)
	{
		return { o.GetX(), o.GetY(), o.GetZ() };
	}

	inline px::Quat ToPX(const glm::quat& o)
	{
		return { o.x, o.y, o.z, o.w };
	}

	inline glm::quat ToGLM(const px::Quat& o)
	{
		return { o.GetW(), o.GetX(), o.GetY(), o.GetZ() };
	}
}

#include "Physics.inl"