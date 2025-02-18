//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Core/Material/Material.h>
#include <Core/Mesh/Mesh.h>
#include <Entity/Entity.h>
#include <Graphics/Buffer/VAO.h>

#include "DrawCall.h"

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(Entity* o, const Reference<Mesh>& mesh);

		void OnInit() override {};
		void OnRender(float delta, Camera*) override;

		GET_CONST(Mesh&, Mesh, *_mesh);

		NODISCARD ALWAYS_INLINE Material& GetMaterial(u8 i) { return *_drawCalls[i].GetMaterial(); }
		NODISCARD ALWAYS_INLINE const Material& GetMaterial(u8 i) const { return *_drawCalls[i].GetMaterial(); }

		void SetMaterial(u8 i, const Reference<Material>& mat);
		void SetMaterial(u8 i, Reference<Material>&& mat);
		void SetNullMaterial(u8 i);

		friend class RendererManager;

	 private:
		Reference<Mesh> _mesh;
		Array<DrawCall> _drawCalls;
	};

	class RendererManager : public ComponentManager<MeshRenderer>
	{
	 public:
		explicit RendererManager(Window* window) : ComponentManager(window), _drawCallManager(window) {};

		void OnRender(float d, Camera* camera) override;

		GET(DrawCallManager&, DrawCallManager, _drawCallManager);

	private:
		DrawCallManager _drawCallManager;
	};
}

#include "MeshRenderer.inl"
