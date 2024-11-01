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

	class PhysicsObject : public Component
	{
	public:
	protected:
		px::Body* _body;
	};

	class PhysicsManager final : public ComponentManager<Component>
	{
	public:
		explicit PhysicsManager(Window* window);

		void Simulate(float delta);

		~PhysicsManager() override;

	private:
		CollisionFilter _filter;
		BroadPhaseFilter _broadFilter;
		BroadPhase _broadPhase;

		SmartPointer<px::Factory> _factory;
		SmartPointer<px::TempAllocatorImpl> _allocator;
		SmartPointer<px::PhysicsSystem> _physics;
		SmartPointer<px::JobSystemThreadPool> _jobSystem;
	};
}

#include "Physics.inl"