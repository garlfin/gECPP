//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Engine/Component/MaterialHolder.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Renderer/Material.h>
#include <Graphics/Buffer/VAO.h>

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(Entity* o, const Reference<API::IVAO>& mesh, const MaterialHolder& mat);

		void OnUpdate(float delta) override {};
		void OnRender(float delta, Camera*) override;

		GET_CONST(API::IVAO&, Mesh, *_mesh);
		GET_CONST(const MaterialHolder&, Materials, *_materialHolder);

		static inline bool CompareVAO(const MeshRenderer&, const MeshRenderer&);
		static inline bool CompareMaterial(const MeshRenderer&, const MeshRenderer&);
		static inline bool CompareLOD(const MeshRenderer&, const MeshRenderer&);

		friend class RendererManager;

	 private:
		Reference<API::IVAO> _mesh;
		RelativePointer<MaterialHolder> _materialHolder;
	};

	class RendererManager : public ComponentManager<MeshRenderer>
	{
	 public:
		using ComponentManager::ComponentManager;

		void OnRender(float d, Camera* camera) override;

		void ReRegister(MeshRenderer&);
	};
}

#include "MeshRenderer.inl"
