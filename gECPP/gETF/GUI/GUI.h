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

		virtual void OnRender(float) = 0;
		virtual void OnInteract(float) = 0;

		GET_CONST(Element*, Parent, _parent);
		GET_CONST(gE::Window*, Window, _window);
		GET(Transform&, Transform, _transform);

	 private:
		Element* _parent;
		gE::Window* _window;
		Transform _transform;
	};

	class Frame : public Element
	{
	 public:
		SERIALIZABLE_PROTO_T;

		void OnRender(float d) override;
		void OnInteract(float d) override;
	};
}
