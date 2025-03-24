//
// Created by scion on 9/27/2024.
//

#pragma once

#include <Core/Manager.h>
#include <Core/Math/Math.h>
#include <Graphics/Buffer/Buffer.h>

namespace gE
{
	class Material;
	class MeshRenderer;
	class DrawCallManager;
	struct DrawCall;

	struct DrawCallCompare
	{
		using is_transparent = void;

		// Less
		NODISCARD ALWAYS_INLINE bool operator()(const DrawCall* a, const DrawCall* b) const;
	};

	struct DrawCall final
	{
		DELETE_OPERATOR_CM(DrawCall);
	public:
		DrawCall(const MeshRenderer&, u8 i);
		DrawCall() = default;

		GET(auto, Iterator, _it);

		GET_CONST(API::IVAO&, VAO, *_vao);
		GET_CONST(const Transform&, Transform, *_transform);
		GET_CONST(Material*, Material, _material);
		GET_CONST(u8, MaterialIndex, _materialIndex);
		GET_CONST(u8, LOD, _lod);

		bool operator<(const DrawCall& b) const;

		~DrawCall();

	private:
		std::set<const DrawCall*, DrawCallCompare>::iterator _it = DEFAULT;

		const Transform* _transform = nullptr;
		API::IVAO* _vao = nullptr;
		Material* _material = nullptr;
		u8 _materialIndex = 0;
		u8 _lod = 0;
	};

	class DrawCallManager final
	{
	public:
		using SET_T = std::set<const DrawCall*, DrawCallCompare>;

		explicit DrawCallManager(Window* window) :
			_indirectDrawBuffer(window, API_MAX_MULTI_DRAW, nullptr, GPU::BufferUsageHint::Dynamic, true)
		{
			_indirectDrawBuffer.Bind(API::BufferTarget::IndirectDrawBuffer);
		}

		void OnRender(float delta, const Camera* camera);

		void Register(const DrawCall*);
		void Remove(const DrawCall*);

		GET_CONST(const API::Buffer<API::IndirectDrawIndexed>&, Draws, _indirectDrawBuffer);

	private:
		API::Buffer<API::IndirectDrawIndexed> _indirectDrawBuffer;
		GPU::IndirectDraw _batches[API_MAX_MULTI_DRAW] DEFAULT;
		SET_T _draws = DEFAULT;
	};
}

#include "DrawCall.inl"
