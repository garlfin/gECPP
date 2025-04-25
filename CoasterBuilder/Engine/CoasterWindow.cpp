//
// Created by scion on 4/10/2025.
//

#include "CoasterWindow.h"

#include <CoasterBuilder/Engine/Entity/Camera.h>
#include <CoasterBuilder/Resource/Shader/Material.h>
#include <Core/Converter/PVR.h>
#include <Entity/Light/DirectionalLight.h>

#include "Entity/Coaster.h"

namespace Coaster
{
    void CoasterBuilderWindow::OnInit()
    {
        Window::OnInit();

        Cubemaps->LoadSkybox(("Resource/Texture/sky.pvr"));

        Array<File> trackFiles;
        Model::ReadGLTFAsFile(this, "CoasterBuilder/Resource/Model/track.glb", Model::GLTFImportSettings(1.f, true), trackFiles);

        for(File& file : trackFiles)
            Assets.AddFile(std::move(file));

        FreeCamera* camera = new FreeCamera(this);
        Cameras.SetCurrentCamera(&camera->GetRenderTarget());

        auto prototypeAlbedo = Assets.AddFile(PVR::ReadAsFile(this, "CoasterBuilder/Resource/Texture/prototype.pvr"))->Cast<API::Texture2D, false>();
        const auto rasterShaderSource = ref_create<ForwardShader>(this, GPU::Shader("Resource/Shader/uber.vert", "CoasterBuilder/Resource/Shader/uber.frag"));
        const auto rasterShader = Assets.AddFile(File(this, "Resource/Shader/Raster.shdr", rasterShaderSource))->Cast<Shader, false>();
        const auto prototypeMaterial = Assets.AddFile(File(this, "Cobble", Material(this, rasterShader, prototypeAlbedo)))->Cast<PBRMaterial, false>();

        constexpr vec3 sunRotation(-31.f, 30.f, 0.f);
        auto* sun = new DirectionalLight(this, 512, 4.f, quat(radians(sunRotation)));
        sun->SetName("Sun");

        Lights->Sun = sun;

        auto defaultCoasterType = ref_create<CoasterType>();
        defaultCoasterType->Name = "Default Coaster";
        defaultCoasterType->Track =
        TrackAppearance{
            Assets.FindFile("CoasterBuilder/Resource/Model/Track.MESH")->Cast<gE::Mesh, false>(),
            prototypeMaterial
        };

        defaultCoasterType->Straight = ref_create<TrackPreset>();
        defaultCoasterType->Straight->Spline =
        {
            { vec3(-GridSize.x / 2.f, 0.f, 0.f), Up },
            { vec3(0.f), Up },
            { vec3(GridSize.x / 2.f, 0.f, 0.f), Up },
        };
        defaultCoasterType->Straight->ExitPosition = vec3(GridSize.x, 0.f, 0.f);

        defaultCoasterType->SmallTurn = ref_create<TrackPreset>();
        defaultCoasterType->SmallTurn->Spline =
        {
            { vec3(-GridSize.x / 2.f, 0.f, 0.f), Up },
            { vec3(0.f), Up },
            { vec3(0.f, 0.f, GridSize.x / 2.f), Up },
        };
        defaultCoasterType->SmallTurn->ExitPosition = vec3(0.f, 0.f, GridSize.x);
        defaultCoasterType->SmallTurn->ExitRotation = -90.f;
        defaultCoasterType->SmallTurn->FlipTransform = scale(mat4(1.f), vec3(1.f, 1.f, -1.f));

        defaultCoasterType->GentleSlope = ref_create<TrackPreset>();
        defaultCoasterType->GentleSlope->Spline =
        {
            { vec3(-GridSize.x / 2.f, 0.f, 0.f), Up },
            { vec3(0.f, GridSize.y / 2.f, 0.f), Up },
            { vec3(GridSize.x / 2.f, GridSize.y, 0.f), Up },
        };
        defaultCoasterType->GentleSlope->ExitPosition = vec3(GridSize.x, GridSize.y, 0);
        defaultCoasterType->GentleSlope->FlipTransform = scale(mat4(1.f), vec3(1.f, -1.f, 1.f));

        TrackPreset& gentleTransition = defaultCoasterType->GentleSlope->TransitionPreset = ptr_create<TrackPreset>();
        gentleTransition.Spline =
        {
            { vec3(-GridSize.x / 2.f, 0.f, 0.f), Up },
            { vec3(0.f), Up },
            { vec3(GridSize.x / 2.f, GridSize.y / 2.f, 0.f), Up },
        };
        gentleTransition.ExitPosition = vec3(GridSize.x, GridSize.y / 2.f, 0);
        gentleTransition.FlipTransform = defaultCoasterType->GentleSlope->FlipTransform;

        new Coaster(this, defaultCoasterType);
    }
}
