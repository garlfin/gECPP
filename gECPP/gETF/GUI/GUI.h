//
// Created by scion on 1/24/2024.
//

#pragma once

#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

namespace gETF::GUI
{
	enum class SliceMode : u8
	{
		Stretch,
		Continuous
	};

	enum class TransformMode : u8
	{
		Absolute,
		Percent
	};

	struct NineSlice
	{
		glm::vec2 Border;
		SliceMode Mode;
		gE::Reference<GL::Texture2D> Texture;
	};

	struct Transform2D
	{
		glm::vec2 Position;
		glm::vec2 Scale;
		float Rotation;
		u16 Layer;
		TransformMode Mode;
	};

	class Renderable
	{
	 public:
		Renderable(gE::Window* window) : Window(window) {};

		GET_CONST(gE::Window*, Window, Window);

		virtual void Render() = 0;

	 protected:
		gE::Window* const Window;
	};

	class Frame : public Renderable
	{
	 public:
		Transform2D Transform;
	};

}
