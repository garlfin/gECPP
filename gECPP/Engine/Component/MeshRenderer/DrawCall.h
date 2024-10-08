//
// Created by scion on 9/27/2024.
//

#pragma once

#include <Engine/Math/Math.h>
#include <Engine/Utility/Manager.h>
#include <Graphics/Buffer/VAO.h>

namespace gE
{
	class Material;
	class MeshRenderer;
	class DrawCallManager;

	struct DrawCall final : public Managed<DrawCall>
	{
	public:
		DrawCall() = default;
		DrawCall(DrawCallManager& manager, const MeshRenderer&, Reference<Material>&&, u8);

		DELETE_OPERATOR_COPY(DrawCall);
		DEFAULT_OPERATOR_MOVE(DrawCall);

		GET_CONST(const MeshRenderer&, Renderer, *_renderer);
		GET_CONST(Material*, Material, _material.Get());
		GET_CONST(u8, SubMesh, _subMesh);

		friend class DrawCallManager;

	private:
		const MeshRenderer* _renderer = nullptr;
		Reference<Material> _material;
		u8 _subMesh = 0;

		LinkedIterator<Managed> _materialIterator;
		LinkedIterator<Managed> _vaoIterator;
		LinkedIterator<Managed> _submeshIterator;
	};

	class DrawCallManager : public Manager<Managed<DrawCall>>
	{
	public:
		using Manager::Manager;

		void OnUpdate(float d) override {};
		void OnRender(float d, Camera* camera) override;

	protected:
		void OnRegister(Managed<DrawCall>& t) override;
	};
}

#include "DrawCall.inl"
