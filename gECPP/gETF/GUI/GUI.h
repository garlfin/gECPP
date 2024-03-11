//
// Created by scion on 1/24/2024.
//

#pragma once

#include "Stylesheet.h"
#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

namespace gETF::UI
{
	class IElement : public Serializable<File>
	{
	 public:
		IElement(gE::Window* w, IElement* p, Transform& t) : _window(w), _parent(p), _transform(&t) {};
		SERIALIZABLE_PROTO_T;

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
	 public:
		Element(gE::Window* w, IElement* p, Transform& t, const T& r, T& s) :
			IElement(w, p, t), _style(s), _referenceStyle(r)
		{}

		SERIALIZABLE_PROTO_T;

		GET(T&, Style, _style);
		GET_CONST(const T&, ReferenceStyle, _referenceStyle);

		typedef T STYLE_T;

	 protected:
		const T* _referenceStyle;
		T _style;
	};

	class Frame : public Element<FrameStyle>
	{
	 public:
		SERIALIZABLE_PROTO_T;
	};

	class Scene : public Serializable<gETF::File>
	{
	 public:
		SERIALIZABLE_PROTO_T;

		std::string Name;
		Array<IElement> Elements;
	};

 	struct UI : public Serializable<gETF::File>
	{
	 public:
		SERIALIZABLE_PROTO_T;

		Array<Frame::STYLE_T> FrameStyles;
		Array<Scene> Scenes;
	};
}

template<class T>
void gETF::UI::Element<T>::Serialize(istream& ptr, const File& settings)
{
	IElement::Serialize(ptr, settings);
}

template<class T>
void gETF::UI::Element<T>::Deserialize(ostream& buf, const gETF::File& settings) const
{
	IElement::Deserialize(buf, settings);
}
