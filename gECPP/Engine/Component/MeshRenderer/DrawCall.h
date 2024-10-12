//
// Created by scion on 9/27/2024.
//

#pragma once

#include <Engine/Math/Math.h>
#include <Engine/Utility/Manager.h>
#include <Graphics/Buffer/Buffer.h>

namespace gE
{
	class Material;
	class MeshRenderer;
	class DrawCallManager;
	struct DrawCall;

	inline bool CompareVAO(const Managed<DrawCall>&, const Managed<DrawCall>&);
	inline bool CompareMaterial(const Managed<DrawCall>&, const Managed<DrawCall>&);
	inline bool CompareSubMesh(const Managed<DrawCall>&, const Managed<DrawCall>&);
	inline bool CompareLOD(const Managed<DrawCall>&, const Managed<DrawCall>&);

	struct DrawCall final : public Managed<DrawCall>
	{
	public:
		DrawCall() = default;
		DrawCall(DrawCallManager& manager, const MeshRenderer&, Reference<Material>&&, u8);

		DELETE_OPERATOR_COPY(DrawCall);
		DEFAULT_OPERATOR_MOVE(DrawCall);

		GET_CONST(API::IVAO&, VAO, *_vao);
		GET_CONST(const Transform&, Transform, *_transform);
		GET_CONST(Material*, Material, _material.Get());
		GET_CONST(u8, MaterialIndex, _subMesh);
		GET_CONST(u8, LOD, _lod);

		friend class DrawCallManager;

	private:
		API::IVAO* _vao;
		const Transform* _transform;
		Reference<Material> _material;
		u8 _subMesh = 0;
		u8 _lod = 0;

		LinkedIterator<Managed> _vaoIterator;
		LinkedIterator<Managed> _materialIterator;
		LinkedIterator<Managed> _subMeshIterator;
		LinkedIterator<Managed> _lodIterator;
	};

	class DrawCallManager : public Manager<Managed<DrawCall>>
	{
	public:
		explicit DrawCallManager(Window* window) : _indirectDrawBuffer(window, API_MAX_MULTI_DRAW)
		{
			_indirectDrawBuffer.Bind(API::BufferTarget::IndirectDrawBuffer);
		}

		void OnUpdate(float d) override {};
		void OnRender(float d, Camera* camera) override;
		ALWAYS_INLINE void UpdateDrawCalls(u64 size = sizeof(API::IndirectDrawIndexed) * API_MAX_MULTI_DRAW, u64 offset = 0) const
		{
			_indirectDrawBuffer.ReplaceData((u8*) IndirectDraws + offset, size, offset);
		}

		API::IndirectDrawIndexed IndirectDraws[API_MAX_MULTI_DRAW];

	protected:
		void OnRegister(Managed<DrawCall>& t) override;

	private:
		API::Buffer<API::IndirectDrawIndexed> _indirectDrawBuffer;

		LinkedList<Managed<DrawCall>> _vaoList;
		LinkedList<Managed<DrawCall>> _materialList;
		LinkedList<Managed<DrawCall>> _subMeshList;
		LinkedList<Managed<DrawCall>> _lodList;
	};
}

#include "DrawCall.inl"
