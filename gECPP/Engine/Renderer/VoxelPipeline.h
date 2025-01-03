//
// Created by scion on 9/19/2023.
//

#pragma once

#include <Engine/Component/Camera/Camera.h>
#include <Engine/Math/Math.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Graphics/Shader/Shader.h>
#include <Graphics/Texture/Texture.h>

#define GE_VOXEL_GRID_RESOLUTION 8
#define GE_VOXEL_MAX_PROBES 64
#define GE_VOXEL_SAMPLE_DIRECTIONS 6

namespace GL
{
	struct Probe
	{
		glm::vec3 Position;
		u32 ID;
		ColorHarmonic Color;
	};

	struct VoxelScene
	{
		glm::vec3 Center;
		float Scale;
		API_ALIGN handle Color;
		u32 ProbeSettings;
		API_ALIGN Probe Probes[GE_VOXEL_MAX_PROBES];
	};
}

namespace gE
{
	class VoxelCapture;
}

namespace gE::VoxelPipeline
{
	struct Buffers
	{
	 public:
		explicit Buffers(Window* window);

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(API::VoxelScene), u64 offset = 0) const
		{
			_voxelBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		API::VoxelScene Scene;

	private:
		API::Buffer<API::VoxelScene> _voxelBuffer;
	};

	struct ProbeSettings
	{
		u8 Resolution;
		glm::u8vec3 GridResolution;
	};

	GLOBAL GPU::Texture ColorFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_RGB10_A2;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.MipCount = 0;
		return tex;
	}();

	GLOBAL GPU::Texture ColorBackFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_RGB10_A2;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.Filter = GPU::FilterMode::Nearest;
		tex.MipCount = 1;
		return tex;
	}();

	GLOBAL GPU::Texture ProbeFormat = ColorBackFormat;

	class Target3D : public RenderTarget<Camera3D>
	{
	 public:
		explicit Target3D(VoxelCapture&, Camera3D&, ProbeSettings);
		GET(API::Texture3D&, Color, _color);
		GET(float, Scale, GetCamera().GetScale());
		GET(VoxelCapture&, Owner, (VoxelCapture&) RenderTarget<Camera3D>::GetOwner());
		GET(ProbeSettings&, ProbeSettings, _probeSettings);

		bool Setup(float d, Camera* camera) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float d) override;

	private:
		API::Texture3D _color, _colorBack;
		ProbeSettings _probeSettings;
	};
}