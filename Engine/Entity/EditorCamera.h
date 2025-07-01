//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Component/Camera/Camera.h>
#include <Component/Physics/CharacterController.h>
#include <Core/Pipeline/VRPipeline.h>
#include <Core/Pipeline/PostProcess/Tonemap.h>
#include <Component/Behavior.h>

namespace gE
{
	inline ICameraSettings FlyCameraSettings
	{
		ClipPlanes(0.1, 100),
		DEFAULT,
	};

	class EditorCameraMovement final : public Behavior
    {
        SERIALIZABLE_PROTO(EditorCameraMovement, Behavior);

    public:
        explicit EditorCameraMovement(Entity* o) :
            Behavior(o)
        {}

        float Speed = 2.0;
        float SpeedMultiplier = 2.f;
        float StandingHeight = 1.75;
        float CrouchingHeight = 0.875;
        float JumpHeight = 1.f;
        float Sensitivity = 0.1f;

        void OnInit() override;
        void OnUpdate(float delta) override;

    private:
        vec3 _rot = DEFAULT;
        RelativePointer<CharacterController> _controller;
        vec3 _dir = DEFAULT;
		vec2 _mousePos = DEFAULT;
    };

	inline void EditorCameraMovement::IDeserialize(istream& in, SETTINGS_T s) {}
	inline void EditorCameraMovement::ISerialize(ostream& out) const {}

	inline REFLECTABLE_ONGUI_IMPL(EditorCameraMovement,
	{
		/*DrawField(ScalarField{ "Speed"sv, ""sv, 0.f, 10.f, 0.01f }, Speed, depth);
		DrawField(ScalarField{ "Speed Multiplier"sv, ""sv, 0.f, 10.f, 0.01f }, SpeedMultiplier, depth);
		DrawField(ScalarField{ "Height"sv, ""sv, 0.f, 10.f, 0.01f }, StandingHeight, depth);
		DrawField(ScalarField{ "Crouching Height"sv, ""sv, 0.f, 10.f, 0.01f }, CrouchingHeight, depth);
		DrawField(ScalarField{ "Jump Height"sv, ""sv, 0.f, 10.f, 0.01f }, JumpHeight, depth);
		DrawField(ScalarField{ "Sensitivity"sv, "Mouse sensitivity"sv, 0.01f, 10.f, 0.001f }, Sensitivity, depth);*/
	});

	REFLECTABLE_FACTORY_IMPL(EditorCameraMovement, inline);

	class EditorCamera final : public Entity
	{
		REFLECTABLE_PROTO(EditorCamera, Entity);

	public:
		explicit EditorCamera(Window* window);

		GET(gE::PerspectiveCamera&, Camera, _camera);
		GET(gE::EditorCameraMovement&, Movement, _movement);
		GET(auto&, Target, _target);
		GET(GL::Texture2D&, Color, _target.GetColor());
		GET(GL::Texture2D&, Depth, _target.GetDepth());

	private:
		EditorCameraMovement _movement;
		PerspectiveCamera _camera;
		DefaultPipeline::Target2D _target;

		PostProcess::PhysicalCameraSettings _physicalCamera = DEFAULT;
		PostProcess::Tonemap _tonemap;
	};

	REFLECTABLE_FACTORY_NO_IMPL(EditorCamera, inline);
	inline REFLECTABLE_ONGUI_IMPL(EditorCamera,
	{
		/*DrawField(Field{ "Camera"sv, ""}, _camera, depth);
		DrawField(Field{ "PhysicalCamera"sv, ""}, _physicalCamera, depth);*/
	});
}
