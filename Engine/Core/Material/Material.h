//
// Created by scion on 9/11/2023.
//

#pragma once

#include <Core/Pointer.h>
#include <Graphics/Shader/Shader.h>

#include "Shader.h"

namespace gE
{
	class Material : public Asset
	{
		REFLECTABLE_PROTO("MAT", Material, Asset, );

	public:
		Material(Window* window, const Reference<Shader>& shader);

		virtual void Bind() const;
		virtual void GetGPUMaterialData(size_t index) const {};
		virtual void FlushMaterialData(size_t size) const {};

		void Free() override {};
		NODISCARD bool IsFree() const override { return true; }

		GET_CONST(Window&, Window, *_window);
		GET(Shader&, Shader, _shader);

		~Material() override = default;

	private:
		Window* _window;
		Reference<Shader> _shader;
	};
}
