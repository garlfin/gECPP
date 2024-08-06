//
// Created by scion on 9/19/2023.
//

#pragma once

#include "Engine/Math/Math.h"
#include "Graphics/Texture/Texture.h"
#include "Graphics/Buffer/Buffer.h"
#include "Engine/Component/Camera/Camera.h"

#define SDF_MAX_TEXTURE 2
#define SDF_MAX_MESH 64
#define SDF_MAX_INSTANCE 128
#define SDF_MAX_DYNAMIC_INSTANCE 32

namespace GL
{
	struct SDFReference
	{
		glm::u16vec3 UV;
		u8 Flags; // TextureID: 6, IsDynamic, ---
		glm::u8vec3 Size;
	};

	struct SDFInstance
	{
		glm::mat4 Transform;
		SDFReference Color;
		u16 Mesh;
	};

	struct SDFNode
	{
		u8 Level; // Traversal aid
		u8 Flags; // HasParent, IsLeafChildA, IsLeafChildB

		u16 Parent;
		u16 ChildA;
		u16 ChildB;

		float Scale;
		glm::i16vec3 Position;
		glm::i16vec3 Bounds;
	};

	struct SDFScene
	{
		handle DistanceTexture[SDF_MAX_TEXTURE];
		handle ColorTexture[SDF_MAX_TEXTURE];

		SDFReference Meshes[SDF_MAX_MESH];
		SDFInstance Instances[SDF_MAX_INSTANCE];
		SDFNode Nodes[SDF_MAX_INSTANCE - 1];
	};
}

namespace gE
{
	class SDFCapture;
}

namespace gE::SDFPipeline
{
	struct Buffers
	{
	 public:
		explicit Buffers(Window* window);

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(API::SDFScene), u64 offset = 0) const
		{
			_sdfBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		API::SDFScene Scene;

	 private:
		API::Buffer<API::SDFScene> _sdfBuffer;
	};

	CONSTEXPR_GLOBAL API::ITextureSettings ColorFormat { GL_RGB10_A2, API::WrapMode::Clamp, API::FilterMode::Linear, 1 };
	CONSTEXPR_GLOBAL API::ITextureSettings SDFFormat { GL_R32F, API::WrapMode::Clamp, API::FilterMode::Nearest, 1 };

	class Target3D : public RenderTarget<Camera3D>
	{
	 public:
		explicit Target3D(SDFCapture&, Camera3D&);

		GET(API::Texture3D&, Color, _color);
		GET(API::Texture3D&, SDF, _sdf);
		GET(API::Texture3D&, SDFBack, _sdfBack);

		GET(float, Scale, GetCamera().GetScale());
		GET(SDFCapture&, Owner, (SDFCapture&) RenderTarget<Camera3D>::GetOwner());

		bool Setup(float d, Camera* camera) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float d) override;

	 private:
		API::Texture3D _color;
		API::Texture3D _sdf, _sdfBack;
	};
}