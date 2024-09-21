//
// Created by scion on 8/9/2023.
//

#pragma once

#include <Engine/Window.h>

namespace gE::VoxelDemo
{
	class DemoWindow final : public Window
	{
	 public:
		explicit DemoWindow(glm::u16vec2 size, const char* name = "VoxelDemo") : Window(size, name) {};

	 private:
		void OnInit() override;
	};
}
