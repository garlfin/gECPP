//
// Created by scion on 1/24/2024.
//

#pragma once

#include "Stylesheet.h"
#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

namespace gETF::UI
{
	class Element : public Serializable<File>
	{
	 public:
		SERIALIZABLE_PROTO_T;

		GET_CONST(Element*, Parent, _parent);
		GET_CONST(gE::Window*, Window, _window);
		GET(Style&, Style, *_style);
		GET(Transform&, GlobalTransform, _globalTransform);
		GET_CONST(u8, ChildCount, _childCount);

	 private:
		Element* _parent;
		gE::Window* _window;
		Style* _style;
		Transform _globalTransform;
		u16 _childCount;
	};

	class Frame : public Element
	{
	 public:
		SERIALIZABLE_PROTO_T;

		GET(FrameStyle&, Style, _style);

	 private:
		FrameStyle _style;
	};
}
