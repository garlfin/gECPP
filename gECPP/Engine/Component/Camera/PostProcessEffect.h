//
// Created by scion on 11/28/2023.
//

#pragma once

#include <Engine/Utility/Macro.h>
#include <Engine/Utility/RelativePointer.h>

#define POSTPROCESS_CONSTRUCTOR(TYPE) TYPE(TARGET_T* target, SETTINGS_T* settings) : PostProcessEffect(target, settings) {}

namespace gE
{
	class Window;

	template<class _TARGET_T>
	class IPostProcessEffect
	{
	 public:
		using TARGET_T = _TARGET_T;
		using TEX_T = typename _TARGET_T::TEX_T;

		explicit IPostProcessEffect(TARGET_T* target) : _target(target) {};

		// If result placed in 'in', return false.
		NODISCARD virtual bool RenderPass(TEX_T& in, TEX_T& out) = 0;

		GET_CONST(const TARGET_T&, Target, *_target);
		GET_CONST(Window&, Window, _target->GetWindow());

		virtual ~IPostProcessEffect() = default;

	private:
		TARGET_T* _target;
	};

	template<class _TARGET_T, class _SETTINGS_T>
	class PostProcessEffect : public IPostProcessEffect<_TARGET_T>
	{
	public:
		using SETTINGS_T = _SETTINGS_T;

		PostProcessEffect(_TARGET_T* target, SETTINGS_T* settings) : IPostProcessEffect<_TARGET_T>(target),
			_settings(settings)
		{}

		GET_SET(SETTINGS_T&, Settings, *_settings);

	private:
		RelativePointer<_SETTINGS_T> _settings;
	};
}