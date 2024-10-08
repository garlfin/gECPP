//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Renderer/Material.h>
#include <Graphics/Buffer/VAO.h>
#include "DrawCall.h"

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(Entity* o, const Reference<API::IVAO>& mesh);

		void OnUpdate(float delta) override {};
		void OnRender(float delta, Camera*) override;

		GET_CONST(API::IVAO&, Mesh, *_mesh);

		static inline bool CompareVAO(const MeshRenderer&, const MeshRenderer&);
		static inline bool CompareMaterial(const MeshRenderer&, const MeshRenderer&);
		static inline bool CompareLOD(const MeshRenderer&, const MeshRenderer&);

		NODISCARD Material& GetMaterial(u8 i) const;

		void SetMaterial(u8 i, const Reference<Material>& mat);
		void SetMaterial(u8 i, Reference<Material>&& mat);
		void SetNullMaterial(u8 i);

		friend class RendererManager;

	 private:
		Reference<API::IVAO> _mesh;
		Array<DrawCall> _drawCalls;
	};

	class RendererManager : public ComponentManager<MeshRenderer>
	{
	 public:
		using ComponentManager::ComponentManager;

		void OnRender(float d, Camera* camera) override;

		GET(DrawCallManager&, DrawCallManager, _drawCallManager);

	private:
		DrawCallManager _drawCallManager;
	};
}

#include "MeshRenderer.inl"
