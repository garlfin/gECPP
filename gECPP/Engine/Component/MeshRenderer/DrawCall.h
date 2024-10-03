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
	class DrawCallManager;

	struct DrawCall final : public Managed<DrawCall>
	{
	public:
		DrawCall() = default;
		explicit DrawCall(DrawCallManager& manager);

		DELETE_OPERATOR_COPY(DrawCall);
		DEFAULT_OPERATOR_MOVE(DrawCall);

		API::VAO* VAO = nullptr;
		Material* Material = nullptr;
		u8 SubMesh = 0;
		u8 LOD = 0;

		friend class DrawCallManager;

	private:
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

		friend struct DrawCall;

	protected:
		void OnRegister(Managed<DrawCall>& t) override {};
	};
}

#include "DrawCall.inl"
