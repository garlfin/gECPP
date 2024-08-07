//
// Created by scion on 1/24/2024.
//

#pragma once

#include "Stylesheet.h"
#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

namespace gETF::UI
{
	class IElement : public Serializable<const Header&>
	{
	 	SERIALIZABLE_PROTO_T(IElement, Serializable<const Header&>);

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
		SERIALIZABLE_PROTO_T(Element, IElement);

	 public:
		Element(gE::Window* w, IElement* p, Transform& t, const T& r, T& s) :
			IElement(w, p, t), _style(s), _referenceStyle(r)
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
		SERIALIZABLE_PROTO_T(Frame, Element<FrameStyle>);
	};

	class Scene : public Serializable<const Header&>
	{
		SERIALIZABLE_PROTO_T(Scene, Serializable<const Header&>);

	 public:
		std::string Name;
		Array<IElement> Elements;
	};

 	struct UI : public Serializable<const Header&>
	{
		SERIALIZABLE_PROTO_T(UI, Serializable<const Header&>);

	 public:
		Array<Frame::STYLE_T> FrameStyles;
		Array<Scene> Scenes;
	};
}

template<class T>
void gETF::UI::Element<T>::ISerialize(istream& ptr, const Header& settings)
{
}

template<class T>
void gETF::UI::Element<T>::IDeserialize(ostream& buf) const
{
}
