//
// Created by scion on 11/27/2024.
//

#include "CharacterController.h"

#include <Engine/Window.h>

namespace gE
{
    CharacterController::CharacterController(Entity* owner, const Physics::CapsuleShape& shape) :
        Component(owner),
        _shape(&GetWindow(), shape),
        _filter(GetOwner().GetLayer()),
        _broadFilter(GetOwner().GetLayer())
    {
        PhysicsManager& manager = GetWindow().GetPhysics();
        px::PhysicsSystem& system = manager.GetSystem();
        const px::PhysicsSettings& physicsSettings = system.GetPhysicsSettings();

        JPH::CharacterVirtualSettings settings = DEFAULT;

        settings.mMass = 80.f; // Average person weight.
        settings.mEnhancedInternalEdgeRemoval = true;

        _controller = ptr_create<px::CharacterVirtual>(&settings, px::Vec3::sZero(), px::Quat::sIdentity(), (u64) this, &system);

        JPH::TempAllocatorImpl tempAllocator(1024);

        _controller->SetShape(
            &_shape.GetJoltShape(),
            physicsSettings.mPenetrationSlop,
            _broadFilter,
            _filter,
            DefaultBodyFilter,
            DefaultShapeFilter,
            manager.GetTempAllocator()
        );
    }
}
