//
// Created by scion on 11/28/2023.
//

#pragma once

#include <Engine/Utility/Macro.h>
#include <Engine/Utility/RelativePointer.h>

namespace gE
{
	// Currently, post process effects don't support per-camera settings.
	// I don't have time to work on this. I'll add it in a later version.

	template<class T>
	class PostProcessEffect
	{
	 public:
		explicit PostProcessEffect(T& target) : _target(target) {};

		virtual void RenderPass(typename T::TEX_T&, typename T::TEX_T&) = 0;

		GET(T&, Target, *_target);

		virtual ~PostProcessEffect() = default;

	 private:
		RelativePointer<T> _target;
	};
}