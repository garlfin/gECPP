//
// Created by scion on 1/24/2024.
//

#pragma once

#include <Engine/Utility/AssetManager.h>
#include <Graphics/Texture/Texture.h>

#include "Stylesheet.h"

#define GE_UI_MAX_STYLES 4

namespace gETF::UI
{
	class Element : public Serializable<>
	{
	 	SERIALIZABLE_PROTO(ELMT, 1, Element, Serializable);

	 public:
		Element(gE::Window* w, Element* p) : _window(w), _parent(p) {};

		GET_CONST(gE::Window*, Window, _window);
		GET_CONST(Element*, Parent, _parent);
		//GET_CONST(const Style* const*, Styles, _styles);
		//GET_CONST(const Style&, EvaluatedStyle, _evaluatedStyle);

	 private:
		gE::Window* _window = DEFAULT;
		Element* _parent = DEFAULT;

	};

	class Frame : public Element
	{
		SERIALIZABLE_PROTO(FRM, 1, Frame, Element);
	};

	class Scene : public Serializable<gE::Window*>
	{
		SERIALIZABLE_PROTO(SCN, 1, Scene, Serializable);

	 public:
		std::string Name;
		Array<Element> Elements;
	};
}
