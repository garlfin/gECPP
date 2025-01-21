//
// Created by scion on 1/24/2024.
//

#pragma once

#include <Engine/Utility/AssetManager.h>
#include <Graphics/Texture/Texture.h>

#include "Stylesheet.h"

namespace gETF::UI
{
	class Element : public Serializable<>
	{
	 	SERIALIZABLE_PROTO("ELMT", 1, Element, Serializable);

	 public:
		Element(gE::Window* w, Element* p = nullptr) : _window(w), _parent(p) {};

		GET_CONST(gE::Window&, Window, *_window);
		GET_CONST(Element*, Parent, _parent);
		GET_CONST(Style&, Styles, *_style);

	 private:
		gE::Window* _window = DEFAULT;
		Element* _parent = DEFAULT;
		Style* _style = DEFAULT;
	};

	class Frame : public Element
	{
		SERIALIZABLE_PROTO("FRM", 1, Frame, Element);
	public:
		Frame(gE::Window* w, Element* p = nullptr);

	private:
		FrameStyle _style = DEFAULT;
	};

	class Scene : public Serializable<gE::Window*>
	{
		SERIALIZABLE_PROTO("SCN", 1, Scene, Serializable);

	 public:
		std::string Name = DEFAULT;
		Array<Element> Elements = DEFAULT;

	private:
	};
}
