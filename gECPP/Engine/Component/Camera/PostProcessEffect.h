//
// Created by scion on 11/28/2023.
//

#pragma once

#include <Engine/Utility/Macro.h>

#define POSTPROCESS_CONSTRUCTOR(TYPE, SETTINGS_T) TYPE(Window* window, SETTINGS_T const & settings) : PostProcessEffect(window, settings) {}

namespace gE
{
	class Window;

	template<class T>
	class IPostProcessEffect
	{
	 public:
		using TEX_T = typename T::TEX_T;
		using TARGET_T = T;

		explicit IPostProcessEffect(const TARGET_T* target) : _target(target) {};

		// If result placed in 'in', return false.
		NODISCARD virtual bool RenderPass(TEX_T& in, TEX_T& out) = 0;

		GET_CONST(const TARGET_T&, Target, *_target);
		GET_CONST(Window&, Window, _target->GetWindow());

		virtual ~IPostProcessEffect() = default;

	private:
		const TARGET_T* _target;
	};

	template<class TEX_T, class SETTINGS_T>
	class PostProcessEffect : public IPostProcessEffect<TEX_T>
	{
	public:
		PostProcessEffect(Window* window, const SETTINGS_T& settings) : IPostProcessEffect<TEX_T>(window),
			_settings(settings)
		{}

		GET_SET(SETTINGS_T&, Settings, _settings);

	private:
		SETTINGS_T _settings;
	};
}