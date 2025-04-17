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

        constexpr glm::vec3 sunRotation(-31.f, 30.f, 0.f);
        auto* sun = new DirectionalLight(this, 512, 10.f, glm::quat(radians(sunRotation)));
        sun->SetName("Sun");
        Lights->Sun = sun;

        auto defaultCoasterType = ref_create<CoasterType>();
        defaultCoasterType->Name = "Default Coaster";

        defaultCoasterType->Straight().Meshes = { {TrackMod::None, Assets.FindFile("CoasterBuilder/Resource/Model/TrackFlat.MESH")->Cast<Mesh, false>() } };
        defaultCoasterType->Straight().Material = prototypeMaterial;
        defaultCoasterType->Straight().ExitPosition = glm::vec3(GridSize.x, 0, 0);
        defaultCoasterType->Straight().Modifications = TrackMod::All;

        defaultCoasterType->SmallTurn().Meshes = { {TrackMod::None, Assets.FindFile("CoasterBuilder/Resource/Model/TrackTurn.MESH")->Cast<Mesh, false>() } };
        defaultCoasterType->SmallTurn().Material = prototypeMaterial;
        defaultCoasterType->SmallTurn().ExitPosition = glm::vec3(GridSize.x, 0, GridSize.x);
        defaultCoasterType->SmallTurn().ExitRotation = 1;
        defaultCoasterType->SmallTurn().Modifications = TrackMod::None;

        auto* coaster = new Coaster(this, defaultCoasterType);
        //auto* station = new Track(this, *coaster, &defaultCoasterType->Straight(), TrackMod::None, nullptr);
        //station->GetRenderer().SetMaterial(0, prototypeMaterial);
    }
}
