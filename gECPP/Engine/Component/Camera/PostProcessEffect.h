//
// Created by scion on 11/28/2023.
//

#pragma once

#include <Engine/Utility/Macro.h>

#define POSTPROCESS_CONSTRUCTOR(TYPE, SETTINGS_T) TYPE(Window* window, SETTINGS_T const & settings) : PostProcessEffect(window, settings) {}

namespace gE
{
	class Window;

	template<class TEX_T>
	class IPostProcessEffect
	{
	 public:
		explicit IPostProcessEffect(Window* window) : _window(window) {};

		virtual void RenderPass(TEX_T& in, TEX_T& out) = 0;

		GET_CONST(Window&, Window, *_window);

		virtual ~IPostProcessEffect() = default;

	private:
		Window* _window;
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