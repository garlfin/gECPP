//
// Created by scion on 1/24/2024.
//

#pragma once

#include <Engine/Utility/AssetManager.h>
#include <Graphics/Texture/Texture.h>

#include "Stylesheet.h"

namespace gETF::UI
{
	class IElement : public Serializable<const Header&>
	{
	 	SERIALIZABLE_PROTO(IELM, 1, IElement, Serializable<const Header&>);

	 public:
		IElement(gE::Window* w, IElement* p, Transform& t) : _window(w), _parent(p), _transform(&t) {};

		GET_CONST(IElement*, Parent, _parent);
		GET_CONST(gE::Window*, Window, _window);
		GET_CONST(const Transform&, Transform, *_transform);

	 private:
		gE::Window* _window;
		IElement* _parent;
		Transform* _transform;
	};

	template<class T>
	class Element : public IElement
	{
		SERIALIZABLE_PROTO(EMNT, 1, Element, IElement);

	 public:
		Element(gE::Window* w, IElement* p, Transform& t, const T& r, T& s) :
			IElement(w, p, t), _referenceStyle(r), _style(s)
		{}

		GET(T&, Style, _style);
		GET_CONST(const T&, ReferenceStyle, _referenceStyle);

		typedef T STYLE_T;

	 protected:
		const T* _referenceStyle;
		T _style;
	};

	class Frame : public Element<FrameStyle>
	{
		SERIALIZABLE_PROTO(FRM, 1, Frame, Element<FrameStyle>);
	};

	class Scene : public Serializable<const Header&>
	{
		SERIALIZABLE_PROTO(SCN, 1, Scene, Serializable<const Header&>);

	 public:
		std::string Name;
		Array<IElement> Elements;
	};

 	struct UI : public Serializable<const Header&>
	{
		SERIALIZABLE_PROTO(UI, 1, UI, Serializable<const Header&>);

	 public:
		Array<Frame::STYLE_T> FrameStyles;
		Array<Scene> Scenes;
	};
}

template<class T>
void gETF::UI::Element<T>::ISerialize(istream& in, const Header&)
{
}

template<class T>
void gETF::UI::Element<T>::IDeserialize(ostream& out) const
{
}
