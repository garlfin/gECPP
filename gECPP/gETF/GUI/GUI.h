//
// Created by scion on 1/24/2024.
//

#pragma once

#include "Stylesheet.h"
#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

namespace gETF::GUI
{
	class Renderable;

	struct InteractiveState
	{
		bool WasHovered : 1;
		bool WasHeld: 1;
	};

	using GUIFunc = void(*)(gE::Window*, Renderable*);

	class Renderable
	{
	 public:
		explicit Renderable(gE::Window* window) : _window(window) {};

		GET_CONST(gE::Window*, Window, _window);

		virtual void Render() = 0;
		virtual void Update() = 0;

	 private:
		gE::Window* const _window;
	};

	class Interactive
	{
	 public:
		explicit Interactive(gE::Window* window) : _window(window) {};

		GET_CONST(gE::Window*, Window, _window);
		GET_CONST(InteractiveState, State, _state);

		GUIFunc OnEnter = nullptr;
		GUIFunc OnHover = nullptr;
		GUIFunc OnExit = nullptr;

		GUIFunc OnClick = nullptr;
		GUIFunc OnHold = nullptr;
		GUIFunc OnRelease = nullptr;

	 private:
		gE::Window* _window;
		InteractiveState _state;
	};

	class Frame : public Renderable, Serializable<>
	{
	 public:
	 	GET_CONST(Renderable*, Parent, Parent);

	 private:
		Renderable* Parent;
	};

}
